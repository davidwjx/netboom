from conans import ConanFile, CMake, tools
from conans.tools import load
import re, os


class InuCommonJavaConan(ConanFile):
    name = "InuCommonJava"
    def set_version(self):
        content = load(os.path.join(self.recipe_folder, "include/Version.h"))
        version = re.search(r"#define INUCOMMON_VERSION_STR (.*)", content).group(1)
        self.version = version.strip().replace('"', '')
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "Conan recipe for InuCommonJava"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "compiler_args"
    exports_sources = "bin/java/*"

    def package(self):
        self.copy("*", dst="lib", src="bin/java")
        