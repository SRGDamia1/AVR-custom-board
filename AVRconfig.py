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


class AVRconfig:
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

        # check for empty values
        self.check_missing_values(self.d)

        # define common properties
        self.name = self.d["board_name"]
        self.version = self.d["package_version"]
        self.version_parsed = Version(self.d["package_version"])
        self.d["package_version_major"] = self.version_parsed.major
        self.d["package_version_minor"] = self.version_parsed.minor
        self.d["package_version_patch"] = self.version_parsed.micro
        self.chip_variant = self.d["chip_variant"]
        self.d["chip_variant_lower"] = self.d["chip_variant"].lower()
        # chip variant is first extra flag
        self.d["extra_flags"] = f"-D__{self.chip_variant}__"
        # add flag for the board name
        self.d[
            "extra_flags"
        ] += f" -D{self.d['vendor_name'].upper()}_{self.d['board_name'].upper()}"

        # add extra extra GCC flags
        self.d["extra_flags"] += (
            " " + config_file["additional_build_flags"]["extra_extra_flags"]
        )
        self.d["extra_flags_pio"] = self.d["extra_flags"].split(" ")
        # convert to json-esque string
        self.d["extra_flags_pio"] = json.dumps(self.d["extra_flags_pio"])

    def check_missing_values(self, dictionary):
        for key, value in dictionary.items():
            if not value:
                print(f"No value provided for {key}")
                raise RuntimeError("Missing configuration parameters")

    # reads template file, does all substitutions from the dictionary, saves result as destination
    # source and destination should be filenames
    def process_file(self, source, destination):

        # open the template file
        with open(source, "r", encoding="UTF-8") as template_file:
            original = template_file.read()

        # make substitutions
        new_data = Template(original).substitute(self.d)

        # write the new file
        with open(destination, "w", encoding="UTF-8") as dest_file:
            dest_file.write(new_data)

        # delete the template file
        os.remove(source)

    def setup_build_directory(self, dirname):
        self.build_directory = dirname
        # self.package_directory = f"{dirname}/{self.version}"
        self.package_directory = f"{dirname}/current"
        # remove old build directory, if it exists
        if os.path.exists(dirname):
            print("Removing old build directory")
            shutil.rmtree(dirname, onerror=remove_readonly)
        # copy the template directory into the build directory
        print("Copying the template directory")
        shutil.copytree("PACKAGE_TEMPLATE", self.package_directory)

        # rename the variants directory
        variants_dir = self.package_directory + "/variants"
        board_variant = f"{variants_dir}/{self.name}"
        os.rename(variants_dir + "/your_variant", board_variant)
        # move the hand written pins_arduino.h files into the board variant directory
        shutil.copy2("board_data/pins_arduino.h", f"{board_variant}")

    # creates boards.txt, platform.txt and README.md files, by processing template files in package directory
    # these are used by the Arduino IDE
    def write_platform_templates(self):
        # rename the pins_arduino.h as a template so we can easily run substitutions in it
        # this makes it easier for users to get version numbers right
        board_variant = f"{self.package_directory}/variants/{self.name}/pins_arduino.h"
        os.rename(
            board_variant,
            board_variant.replace("/pins_arduino.h", "/pins_arduino_TEMPLATE.h"),
        )

        # Run substitutions in all _TEMPLATE files
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

        # rename the pio boards file
        boards_dir = self.package_directory + "/boards"
        pio_boards_json_name = (
            boards_dir
            + "/"
            + self.d["vendor_name"]
            + "_"
            + self.d["board_name"]
            + ".json"
        )
        os.rename(f"{boards_dir}/pio_board.json", pio_boards_json_name)

    # compress already constructed package directory into a zip archive and
    # record archive size and SHA256 checksum
    def package_archive(self):
        # archive_filename = f"{self.build_directory}/{self.name}-{self.version}"
        archive_filename = f"{self.build_directory}/{self.d['package_name'].replace(' ','').lower()}-{self.version}"
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
            "boards": [{"name": self.d["board_name_long"]}],
            "toolsDependencies": [],
        }
        # add the new platform entry to the package
        existing_platforms.append(avr_current)
        packages["packages"][0]["platforms"] = copy.deepcopy(existing_platforms)

        # now save to json
        indexfile_name = (
            self.build_directory + "/package_" + self.d["vendor_name"] + "_index.json"
        )
        with open(indexfile_name, "w", encoding="UTF-8") as indexfile:
            json.dump(packages, indexfile, indent=2)

# cSpell:words esque AVRconfig
