from conans import ConanFile, CMake, tools
from conans.tools import load
import re, os


class FWtargetConan(ConanFile):
    name = "fw_target"
    def set_version(self):
        content = load(os.path.join(self.recipe_folder, "Version.h"))
        version = re.search(r"#define CM_VERSION_STR (.*)", content).group(1)
        self.version = version.strip().replace('"', '')
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "Conan recipe for FW target"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "compiler_args"
    exports_sources = "uploadToConan/nu4100/target/bootall/boot/boot0/*"

    def package(self):
        self.copy("*", dst="nu4100", src="uploadToConan/nu4100/target/bootall/boot/boot0")
