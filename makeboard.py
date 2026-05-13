#!/usr/bin/env python3
import AVRconfig
import os
import shutil

# Read all board configuration data
print("Reading board config...")
board = AVRconfig.AVRconfig("board_data/board-config.ini")

# %%
# setup build directory and move there all source files
print("\nCopying sources to build directory...")
board.setup_build_directory("build")

# Customize the various template files
print("\nCustomizing all template files")
board.write_platform_templates()

# compressing directory into zip archive
print("\nCompressing the package archive")
board.package_archive()

# create json file
print("\nCreating json index file")
board.write_index_json()


# %%
print("\nAll done!")
