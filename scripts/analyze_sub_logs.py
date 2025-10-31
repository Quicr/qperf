#!/usr/bin/env python3
# -----------------------------------------------------------------------------------------
# Simple program to analyze the qperf subscriber result log files.
#
# Requirements:
#       pip3 install click
# -----------------------------------------------------------------------------------------

import logging
import click
import time
import traceback
import os

logging.basicConfig(format='%(asctime)s | %(levelname)-8s | %(name)s[%(lineno)s] %(threadName)s | %(message)s',
                    level=logging.INFO)
LOG = logging.getLogger("analyze_sub_logs")

PATH = "./"


def process_sub_logs_path(path):
    directory = os.fsencode(path)

    num_delayed = 0
    num_lost_objects = 0
    num_not_completed = 0

    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        if filename.startswith("t_") and filename.endswith("logs.txt"):
            # print(os.path.join(directory, filename))
            file = os.path.join(path, filename)
            LOG.debug(f"Processing file: {file}")

            complete = False
            with open(file, "r") as f:
                for line in f.readlines():
                    if "OR COMPLETE, " in line:
                        complete = True
                        csv = line.split("OR COMPLETE, ", maxsplit=1)[1].split(", ")

                        if len(csv) >= 19:
                            track_id = csv[0]
                            test_name = csv[1]
                            delta_objects = int(csv[17])
                            over_multiplier = int(csv[18].strip())

                            if over_multiplier > 1 or delta_objects != 0:
                                if over_multiplier > 1:
                                    num_delayed += 1
                                if delta_objects != 0:
                                    num_lost_objects += 1

                                LOG.info(f"id: {track_id} track name: '{test_name}' delta_objects: {delta_objects}  over multiplier: {over_multiplier}")
                            else:
                                LOG.debug(f"id: {track_id} track name: '{test_name}' delta_objects: {delta_objects}  over multiplier: {over_multiplier}")
                        else:
                            LOG.info(f"Skipping line csv length: {len(csv)}, expected 19")

            if not complete:
                num_not_completed += 1
        else:
            continue
    # End of for loop through all files

    if num_delayed > 0:
        LOG.warning(f"ANALYSIS: {num_delayed} subscriber tracks were delayed 2 or more times the expected interval")
    if num_lost_objects:
        LOG.warning(f"ANALYSIS: {num_lost_objects} subscriber tracks had lost objects")
    if num_not_completed:
        LOG.warning(f"ANALYSIS: {num_not_completed} subscribers did not complete")

    if num_delayed == 0 and num_lost_objects == 0 and num_not_completed == 0:
        LOG.info("ANALYSIS: No issues found")

@click.command(context_settings=dict(help_option_names=['-h', '--help'], max_content_width=200))
@click.option('-p', '--path', 'path',
              help="Directory/path of where the qperf log files are located", metavar='<path>', default=PATH)
@click.option('-d', '--debug', 'debug',
              help="Enable debug logging",
              is_flag=True, default=False)
def main(path, debug):
    if debug:
        LOG.setLevel(logging.DEBUG)

    LOG.info(f"Reading all qperf subscriber log files in directory {path}")

    process_sub_logs_path(path)

if __name__ == '__main__':
    main()
