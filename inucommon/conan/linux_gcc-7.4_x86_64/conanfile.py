from conans import ConanFile, CMake, tools
from conans.tools import load
import re, os


class InuCommonConan(ConanFile):
    name = "inucommon_linux_gcc-7.4_x86_64"
    def set_version(self):
        content = load(os.path.join(self.recipe_folder, "include/Version.h"))
        version = re.search(r"#define INUCOMMON_VERSION_STR (.*)", content).group(1)
        self.version = version.strip().replace('"', '')
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "Conan recipe for InuCommon for Linux x86_64"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False}
    generators = "compiler_args"
    exports_sources = "include/*", "src/*", "bin/*", "cmake/*", "!src/out/*"

    def package(self):
        self.copy("*", dst="include", src="include")
        self.copy("*.h", dst="include", src="src/AlgWebcamIntegration")
        self.copy("config.h", dst="include", src="src/InuCommonAlgo/AlgStaticTemporalFilter")
        self.copy("*", dst="bin", src="bin/linux_gcc-7.4_x86_64")
        self.copy("conaninfo.txt", dst="dependencies", src="src/Conanfiles")
        self.copy("InuModelDB.csv", dst="config", src="src/InuSensorsDB")