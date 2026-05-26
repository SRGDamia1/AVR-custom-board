"""
Main class to keep configuration data for AVR board
* Author(s): Alexander Kirillov
* Version: 4.0
"""

import os
import copy
import shutil
import stat
import configparser
from string import Template
import hashlib
import json
from datetime import date
import requests
from packaging.version import Version


# Cloning the uf2 directory as part of the build creates read-only files which rmtree cannot
# delete without a helper.
def remove_readonly(func, path, _):
    "Clear the readonly bit and reattempt the removal"
    os.chmod(path, stat.S_IWRITE)
    func(path)


class AVRConfig:
    # constructor
    def __init__(self, filename):
        # dictionary containing all config data
        self.d = {}
        self.d["build_date"] = date.today().isoformat()
        # read all values from main sections of config file
        config_file = configparser.ConfigParser()
        config_file.read(filename)
        for s in ["hardware", "names"]:
            for key, value in config_file[s].items():
                self.d[key] = value
                if key == "board_define_name" and value == "":
                    self.d["board_define_name"] = (
                        self.d["board_name"].lower().replace(" ", "_")
                    )

        # check for empty values
        self.check_missing_values(self.d)

        # define common properties
        self.name = self.d["board_name"]
        self.d["board_name_upper"] = self.d["board_name"].upper()
        self.board_version = self.d["board_version"]
        board_version_parsed = Version(self.d["board_version"])
        self.d["board_version_major"] = board_version_parsed.major
        self.d["board_version_minor"] = board_version_parsed.minor
        self.d["board_version_patch"] = board_version_parsed.micro
        self.chip_variant = self.d["chip_variant"]
        self.d["chip_variant_lower"] = self.d["chip_variant"].lower()
        # chip variant is first extra flag
        self.d["extra_flags"] = f"-D__{self.chip_variant}__"

        # add extra extra GCC flags
        self.d["extra_flags"] += (
            " " + config_file["additional_build_flags"]["extra_extra_flags"]
        )

    def check_missing_values(self, dictionary):
        for key, value in dictionary.items():
            if not value:
                print(f"No value provided for {key}")
                raise RuntimeError("Missing configuration parameters")


class AVRPackage:
    # constructor
    def __init__(self, dirname):
        print(f"Reading package config from {dirname}")
        self.config_directory = dirname
        self.boards_config: list[AVRConfig] = []
        # dictionary containing all config data
        self.d = {}
        self.d["build_date"] = date.today().isoformat()
        # read all values from main sections of config file
        config_file = configparser.ConfigParser()
        config_file.read(os.path.join(dirname, "package-config.ini"))
        for s in ["vendor", "package"]:
            for key, value in config_file[s].items():
                self.d[key] = value
                if key == "package_define_name" and value == "":
                    self.d["package_define_name"] = (
                        self.d["package_name"].lower().replace(" ", "_")
                    )

        # check for empty values
        self.check_missing_values(self.d)

        # define common properties
        self.package_version = self.d["package_version"]
        package_version_parsed = Version(self.d["package_version"])
        self.d["package_version_major"] = package_version_parsed.major
        self.d["package_version_minor"] = package_version_parsed.minor
        self.d["package_version_patch"] = package_version_parsed.micro

        # read all board configuration data
        print("Reading board configs...")
        self.read_board_configs()

    def check_missing_values(self, dictionary):
        for key, value in dictionary.items():
            if not value:
                print(f"No value provided for {key}")
                raise RuntimeError("Missing configuration parameters")

    def read_board_configs(self):
        for board_dir in os.listdir(self.config_directory):
            if (
                not os.path.isdir(os.path.join(self.config_directory, board_dir))
                or "your-variant" in board_dir
            ):
                continue
            board_config_path = os.path.join(
                self.config_directory, board_dir, "board-config.ini"
            )
            print(f"Looking for board config at {board_config_path}")
            if os.path.isfile(board_config_path) and "EXAMPLE" not in board_config_path:
                print(f"Reading config for board {board_dir}")
                board_config = AVRConfig(board_config_path)
                board_config.d["board_dir"] = os.path.join(
                    self.config_directory, board_dir
                )
                # add flag for the board name
                board_config.d[
                    "extra_flags"
                ] += f" -D{self.d['vendor_name'].upper()}_{board_config.d['board_name_upper']}"
                board_config.d["extra_flags_pio"] = board_config.d["extra_flags"].split(
                    " "
                )
                # convert to json-esque string
                board_config.d["extra_flags_pio"] = json.dumps(
                    board_config.d["extra_flags_pio"]
                )
                self.boards_config.append(board_config)
            else:
                print(f"No config file found for board {board_dir}, skipping.")

    # reads template file, does all substitutions from the dictionary, saves result as destination
    # source and destination should be filenames
    def process_file(self, source, destination, sub_dict=None):
        print(f"Processing template file {source} to create {destination}")
        if sub_dict is None:
            sub_dict = self.d

        # open the template file
        with open(source, "r", encoding="UTF-8") as template_file:
            original = template_file.read()

        # make substitutions
        new_data = Template(original).substitute(sub_dict)

        # write the new file
        with open(destination, "w", encoding="UTF-8") as dest_file:
            dest_file.write(new_data)

        # delete the template file
        os.remove(source)

    def setup_build_directory(self):
        # directory for the built package
        self.build_directory = os.path.join(
            os.path.dirname(self.config_directory), "build"
        )
        self.package_directory = os.path.join(self.build_directory, "current")
        print(f"Setting up build directory at {self.package_directory}")
        # remove old build directory, if it exists
        print("Checking for and deleting the content of existing stale build directory")
        if os.path.exists(self.build_directory):
            print("Removing old build directory")
            shutil.rmtree(self.build_directory, onerror=remove_readonly)

        # copy the template directory into the build directory
        print("Copying the template directory")
        shutil.copytree("PACKAGE_TEMPLATE", self.package_directory)

        # copy the variants directories
        variants_dir = os.path.join(self.package_directory, "variants")
        for board in self.boards_config:
            dest_board_variant = os.path.join(variants_dir, board.name)
            os.makedirs(dest_board_variant, exist_ok=True)
            print(
                f"Copying variant files for board {board.name} from {board.d['board_dir']} into build directory at {dest_board_variant}"
            )
            if os.path.isfile(os.path.join(board.d["board_dir"], "pins_arduino.h")):
                # move the hand written pins_arduino.h files into the board variant directory
                shutil.copy2(
                    os.path.join(board.d["board_dir"], "pins_arduino.h"),
                    os.path.join(dest_board_variant, "pins_arduino.h"),
                )
            else:
                print(
                    f"No pins_arduino.h file found for board {board.name} in {board.d['board_dir']}, skipping copying of this file."
                )

    # creates platform.txt, version and README.md files, by processing template files in package directory
    # these are used by the Arduino IDE
    def write_platform_templates(self):
        # create template sub-directories for each board and copy the relevant template files into them
        board_files_to_copy = [
            {
                "template_dir": os.path.join(self.package_directory, "boards"),
                "src_file": "pio_board_TEMPLATE.json",
            },
            {"template_dir": self.package_directory, "src_file": "boards_TEMPLATE.txt"},
            {
                "template_dir": self.package_directory,
                "src_file": "VARIANT_VERSION_TEMPLATE.h",
            },
        ]
        for board in self.boards_config:
            print(f"Customizing template files for board {board.name}")
            for file_info in board_files_to_copy:
                dest_file = (
                    file_info["src_file"]
                    .replace("_TEMPLATE", "_" + board.name + "_TEMPLATE")
                    .replace("pio_board", self.d["vendor_name"])
                )

                # make a **copy** of the template
                shutil.copy2(
                    os.path.join(file_info["template_dir"], file_info["src_file"]),
                    os.path.join(file_info["template_dir"], dest_file),
                )
                # process the template file
                self.process_file(
                    os.path.join(file_info["template_dir"], dest_file),
                    os.path.join(
                        file_info["template_dir"], dest_file.replace("_TEMPLATE", "")
                    ),
                    board.d | self.d,
                )

            # after processing the other templates, grab the variant version macros from the generated version file to insert into the pins_arduino.h file for the board
            with open(
                os.path.join(
                    self.package_directory, "VARIANT_VERSION_" + board.name + ".h"
                ),
                "r",
                encoding="UTF-8",
            ) as version_file:
                board.d["variant_version_macros"] = version_file.read()
            os.remove(
                os.path.join(
                    self.package_directory, "VARIANT_VERSION_" + board.name + ".h"
                )
            )

            # find the pin definition file for the board
            variant_dir = os.path.join(self.package_directory, "variants", board.name)
            # temporarily rename the pin definition file
            os.rename(
                os.path.join(variant_dir, "pins_arduino.h"),
                os.path.join(variant_dir, "pins_arduino_TEMPLATE.h"),
            )
            # process the template file to create the actual pins_arduino.h file for the board
            self.process_file(
                os.path.join(variant_dir, "pins_arduino_TEMPLATE.h"),
                os.path.join(variant_dir, "pins_arduino.h"),
                board.d | self.d,
            )

        # delete the template files we made copies of instead of simply moving
        print("Deleting template files that were copied for processing")
        for file_info in board_files_to_copy:
            os.remove(os.path.join(file_info["template_dir"], file_info["src_file"]))

        # combine the boards template files into a single file with all boards, and delete the individual board template files
        print(
            "Combining individual board template files into a single boards.txt file and deleting the individual files"
        )
        combined_boards = []
        for board in self.boards_config:
            print(
                f"Opening file for board {board.name} at {os.path.join(self.package_directory, f'boards_{board.name}.txt')}"
            )
            with open(
                os.path.join(self.package_directory, f"boards_{board.name}.txt"),
                "r",
                encoding="UTF-8",
            ) as board_file:
                combined_boards.append(board_file.read())
            combined_boards.append("\n")
            print(
                f"Deleting individual board file for board {board.name} at {os.path.join(self.package_directory, f'boards_{board.name}.txt')}"
            )
            os.remove(os.path.join(self.package_directory, f"boards_{board.name}.txt"))
        with open(
            os.path.join(self.package_directory, "boards.txt"), "w", encoding="UTF-8"
        ) as combined_file:
            combined_file.write("\n".join(combined_boards))

        # Run substitutions in all remaining _TEMPLATE files in the package directory
        template_src_files = [
            os.path.join(dp, f)
            for dp, dn, filenames in os.walk(self.package_directory)
            for f in filenames
            if "_TEMPLATE" in f
        ]
        for file_name in template_src_files:
            if os.path.isfile(file_name):
                print(f"Processing template for {file_name}")
                self.process_file(
                    file_name,
                    file_name.replace("_TEMPLATE", ""),
                )

    # compress already constructed package directory into a zip archive and
    # record archive size and SHA256 checksum
    def package_archive(self):
        # archive_filename = f"{self.config_directory}/{self.name}-{self.version}"
        archive_filename = os.path.join(
            self.build_directory,
            f"{self.d['package_name'].replace(' ','').lower()}-{self.package_version}",
        )
        print(f"Creating package archive at {archive_filename}.zip")
        zip_archive = shutil.make_archive(
            archive_filename,
            "zip",
            root_dir=self.build_directory,
            # base_dir=self.version,
            base_dir="current",
        )
        archive_size = os.path.getsize(zip_archive)
        # compute hash:
        with open(zip_archive, "rb") as f:
            bytes = f.read()  # read entire file as bytes
            hash = hashlib.sha256(bytes).hexdigest()

        print(
            f"Created package archive, size {archive_size} bytes,\n SHA256 hash: {hash}"
        )
        # add the info to dictionary
        self.d["archive_filename"] = archive_filename
        self.d["archive_size"] = archive_size
        self.d["archive_checksum"] = hash

    # write json index file
    def write_index_json(self):
        # see structure specifications here: https://arduino.github.io/arduino-cli/1.4/package_index_json-specification/

        # create the package structure
        package = {
            "name": self.d["vendor_name_long"],
            "maintainer": self.d["maintainer_name"],
            "websiteURL": self.d["info_url"],
            "email": self.d["vendor_email"],
            "help": {"online": self.d["help_url"]},
            "platforms": [],
            "tools": [],
        }

        # read the exiting index file if it exists, to preserve previous versions
        if "package_index_file" in self.d and os.path.exists(
            self.d["package_index_file"]
        ):
            with open(self.d["package_index_file"], "r", encoding="UTF-8") as indexfile:
                print(
                    f"Found existing package index at {self.d['package_index_file']}, reading it to preserve previous versions"
                )
                packages = json.load(indexfile)
        elif "package_index_url" in self.d:
            response = requests.get(self.d["package_index_url"])
            if response.status_code == 200:
                print(
                    f"Found existing package index at {self.d['package_index_url']}, reading it to preserve previous versions"
                )
                packages = response.json()
            else:
                print(
                    f"No existing package index found at {self.d['package_index_url']}, creating a new one"
                )
                packages = {"packages": [copy.deepcopy(package)]}
        else:
            print("No existing package index found, creating a new one")
            packages = {"packages": [copy.deepcopy(package)]}

        existing_platforms: list = []
        if "packages" in packages and len(packages["packages"]) > 0:
            existing_package = copy.deepcopy(packages["packages"][0])
            # verify that the basic package info matches the existing one if it exists
            if (
                existing_package["name"] != package["name"]
                or existing_package["maintainer"] != package["maintainer"]
                or existing_package["websiteURL"] != package["websiteURL"]
                or existing_package["email"] != package["email"]
            ):
                raise RuntimeError(
                    "Existing package index has different basic info (name, maintainer, websiteURL, or email). Please resolve this conflict before proceeding."
                )
            # Remove any outdated platform entries for the current version if they exist, to avoid conflicts with the new platform entry we're adding below.
            # NOTE: According to the specification, 3rd party vendors should use a single package within the package index.
            if "platforms" in existing_package:
                existing_platforms = existing_package["platforms"]
                for idx, platform in enumerate(existing_platforms):
                    if (
                        platform["name"] == self.d["package_name"]
                        and platform["version"] == self.d["package_version"]
                    ):
                        print(
                            f"Found outdated platform info for version {self.d['package_version']}, removing it."
                        )
                        existing_platforms.pop(idx)

        # let's create the current version of avr platform
        avr_current = {
            "name": self.d["package_name"],
            "architecture": "avr",
            "version": self.d["package_version"],
            "category": "Contributed",
            "url": self.d["package_url"]
            + self.d["archive_filename"].replace("build/", "")
            + ".zip",
            "archiveFileName": self.d["archive_filename"].replace("build/", "")
            + ".zip",
            "checksum": "SHA-256:" + self.d["archive_checksum"],
            "size": self.d["archive_size"],
            "boards": [],
            "toolsDependencies": [],
        }
        for board in self.boards_config:
            avr_current["boards"].append({"name": board.d["board_name"]})
        # add the new platform entry to the package
        existing_platforms.append(avr_current)
        packages["packages"][0]["platforms"] = copy.deepcopy(existing_platforms)

        # now save to json
        indexfile_name = (
            self.build_directory + "/package_" + self.d["vendor_name"] + "_index.json"
        )
        with open(indexfile_name, "w", encoding="UTF-8") as indexfile:
            json.dump(packages, indexfile, indent=2)


# cSpell:words esque AVRConfig
