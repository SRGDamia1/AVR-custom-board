#!/usr/bin/env python3
import AVRConfig
import os
import shutil

# Read the package configuration file
# this will also read all of the board config files and store them in the AVRPackage object
print("Reading board config...")
package = AVRConfig.AVRPackage("board_data")

# %%
# setup build directory and move there all source files
print("\nCopying sources to build directory...")
package.setup_build_directory()

# Customize the various template files
print("\nCustomizing all template files")
package.write_platform_templates()

# compressing directory into zip archive
print("\nCompressing the package archive")
package.package_archive()

# create json file
print("\nCreating json index file")
package.write_index_json()


# %%
print("\nAll done!")
