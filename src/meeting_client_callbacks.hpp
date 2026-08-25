#pragma once

#include "client_callbacks.hpp"
#include "publisher_track_handler.hpp"
#include "subscriber_track_handler.hpp"

#include <quicr/utilities/defer.h>

#include <chrono>
#include <memory>
#include <string>

namespace moqbench {

    class PerfMeetingClientCallbacks : public PerfClientCallbacks
    {
      public:
        PerfMeetingClientCallbacks(const std::string& configfile,
                                   std::uint32_t meeting_id,
                                   std::uint32_t instances,
                                   std::uint32_t instance_identifier,
                                   std::uint64_t timeout_grace_ms)
          : PerfClientCallbacks(configfile)
          , meeting_id_(meeting_id)
          , instance_id_(instance_identifier)
          , instances_(instances)
          , timeout_grace_ms_(timeout_grace_ms)
        {
        }

        quicr::Reply<const quicr::PublishResponse, quicr::PublishErrorCode> PublishReceived(
          [[maybe_unused]] const std::shared_ptr<quicr::Session>& session,
          [[maybe_unused]] std::uint64_t request_id,
          const quicr::PublishAttributes& publish_attributes,
          [[maybe_unused]] std::weak_ptr<quicr::SubscribeNamespaceHandler> sub_ns_handler) override
        {
            for (const auto& handler : sub_track_handlers_) {
                const auto tfn = handler->GetFullTrackName();
                const auto ns = tfn.name_space;

                if (ns == publish_attributes.track_full_name.name_space) {
                    std::ostringstream ns_str;
                    auto ns_entries = ns.GetEntries();

                    for (const auto entry : ns_entries) {
                        ns_str << '/';
                        ns_str << std::string(entry.begin(), entry.end());
                    }

                    SPDLOG_INFO("Publish Received matching Subscribe track; test name: {} ns: {} name: {} forward: {}",
                                handler->TestName(),
                                ns_str.str(),
                                std::string(tfn.name.begin(), tfn.name.end()),
                                static_cast<int>(publish_attributes.forward));

                    return quicr::PublishResponse{ .attributes = { .forward = true }, handler };
                }
            }

            return quicr::Unexpected<quicr::Error<quicr::PublishErrorCode>>(quicr::PublishErrorCode::kInternalError,
                                                                            "No matching subscribe track");
        }

        void StatusChanged(const std::shared_ptr<quicr::Session>& session, quicr::Session::Status status) override
        {
            switch (status) {
                case quicr::Session::Status::kReady:
                    SPDLOG_INFO("Client status - kReady");
                    inif_.load(config_file_);

                    for (const auto& [section_name, _] : inif_) {
                        auto pub_handler = pub_track_handlers_.emplace_back(
                          PerfPublishTrackHandler::Create(section_name, inif_, instance_id_ + (meeting_id_ * 1000)));
                        session->PublishTrack(pub_handler);
                    }

                    for (std::uint32_t i = 1; i <= instances_; ++i) {
                        if (i == instance_id_) {
                            continue;
                        }

                        for (const auto& [section_name, _] : inif_) {
                            auto sub_handler = sub_track_handlers_.emplace_back(PerfSubscribeTrackHandler::Create(
                              section_name, inif_, i + (meeting_id_ * 1000), timeout_grace_ms_));

                            sub_handler->SetPublishInitiated();

                            auto sub_ns =
                              quicr::SubscribeNamespaceHandler::Create(sub_handler->GetFullTrackName().name_space,
                                                                       quicr::SubscribeNamespaceHandler::Mode::kTracks);
                            session->SubscribeNamespace(sub_ns);
                        }
                    }

                    break;
                case quicr::Session::Status::kNotReady:
                    SPDLOG_INFO("Client status - kNotReady");
                    break;
                case quicr::Session::Status::kConnecting:
                    SPDLOG_INFO("Client status - kConnecting");
                    break;
                case quicr::Session::Status::kNotConnected:
                    SPDLOG_INFO("Client status - kNotConnected");
                    exit(0);
                case quicr::Session::Status::kPendingServerSetup:
                    SPDLOG_INFO("Client status - kPendingSeverSetup");
                    break;

                case quicr::Session::Status::kFailedToConnect:
                    SPDLOG_ERROR("Client status - kFailedToConnect");
                    terminate_ = true;
                    break;
                case quicr::Session::Status::kInternalError:
                    SPDLOG_ERROR("Client status - kInternalError");
                    terminate_ = true;
                    break;
                case quicr::Session::Status::kInvalidParams:
                    SPDLOG_ERROR("Client status - kInvalidParams");
                    terminate_ = true;
                    break;
                default:
                    SPDLOG_ERROR("Connection failed {0}", static_cast<int>(status));
                    terminate_ = true;
                    break;
            }
        }

        bool HandlersComplete() override
        {
            std::lock_guard<std::mutex> _(mutex_);
            defer(std::this_thread::sleep_for(std::chrono::milliseconds(100)));

            if (sub_track_handlers_.empty() || pub_track_handlers_.empty()) {
                return false;
            }

            for (auto handler : pub_track_handlers_) {
                if (!handler->IsComplete()) {
                    return false;
                }
            }

            for (auto handler : sub_track_handlers_) {
                if (!handler->IsComplete() && !handler->HasTimedOut()) {
                    return false;
                }
            }

            return true;
        }

        void Terminate(const std::shared_ptr<quicr::Session>& session) override
        {
            std::lock_guard<std::mutex> _(mutex_);

            for (auto handler : sub_track_handlers_) {
                SPDLOG_INFO("unsubscribe track {}", handler->TestName());
                session->UnsubscribeTrack(handler);
            }

            for (auto handler : pub_track_handlers_) {
                handler->StopWriter();
                session->UnpublishTrack(handler);
            }

            terminate_ = true;
        }

      private:
        std::uint32_t meeting_id_;
        std::uint32_t instance_id_;
        std::uint32_t instances_;
        std::uint64_t timeout_grace_ms_;

        std::vector<std::shared_ptr<PerfSubscribeTrackHandler>> sub_track_handlers_;
        std::vector<std::shared_ptr<PerfPublishTrackHandler>> pub_track_handlers_;
    };

}
