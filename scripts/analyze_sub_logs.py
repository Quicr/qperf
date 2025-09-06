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

    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        if filename.startswith("t_") and filename.endswith("logs.txt"):
            # print(os.path.join(directory, filename))
            file = os.path.join(path, filename)
            LOG.info(f"Processing file: {file}")

            with open(file, "r") as f:
                for line in f.readlines():
                    if "OR COMPLETE, " in line:
                        csv = line.split("OR COMPLETE, ", maxsplit=1)[1].split(", ")

                        if len(csv) >= 19:
                            track_id = csv[0]
                            test_name = csv[1]
                            delta_objects = csv[17]
                            over_multiplier = csv[18].strip()
                            LOG.info(f"id: {track_id} track name: '{test_name}' delta_objects: {delta_objects}  over multiplier: {over_multiplier}")
                        else:
                            LOG.info(f"Skipping line csv length: {len(csv)}, expected 19")
        else:
            continue


@click.command(context_settings=dict(help_option_names=['-h', '--help'], max_content_width=200))
@click.option('-p', '--path', 'path',
              help="Directory/path of where the qperf log files are located", metavar='<path>', default=PATH)
#@click.option('-b', '--bool', 'bool', default=False,
#              is_flag=True, help="Bool value")
def main(path):
    LOG.info(f"Loop through all qperf log files in directory {path}")

    process_sub_logs_path(path)

if __name__ == '__main__':
    main()
