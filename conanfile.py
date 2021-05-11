import os
from conans import ConanFile, CMake

class IgeConan(ConanFile):
    name = 'igeCreator'
    version = '0.0.5'
    license = "MIT"
    author = "Indi Games"
    url = "https://github.com/indigames"
    description = name + " library for IGE Engine"
    topics = ("#Python", "#IGE", "#Games")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"
    exports_sources = ""
    requires = []
    short_paths = True

    def requirements(self):
        self.requires("Python/3.7.6@ige/test")
        self.requires("numpy/1.19.5@ige/test")
        self.requires("SDL/2.0.11@ige/test")
        self.requires("stb/1.0.0@ige/test")
        self.requires("bullet/3.0.8@ige/test")
        self.requires("igeBullet/0.4.61@ige/test")
        self.requires("taskflow/3.1.0@ige/test")
        self.requires("tracy/0.6.6@ige/test")
        self.requires("igeSound/0.1.0@ige/test")
        self.requires("igeEffekseer/0.0.18@ige/test")
        self.requires("igeNavigation/0.0.1@ige/test")
        self.requires("openssl/1.1.1c@ige/test")
        if (self.settings.os == "Windows") and (self.name != "zlib"):
            self.requires("zlib/1.2.11@ige/test")

    def package(self):
        output_dir = os.path.normpath(os.path.relpath(os.environ['OUTPUT_DIR'], os.environ['PROJECT_DIR']))
        self.copy("*", dst="include", src=os.path.join(output_dir, "include"))
        if self.settings.os == "Windows":
            if self.settings.arch == "x86":
                self.copy("*", dst="lib", src=os.path.join(output_dir, "libs/windows/x86"))
            else:
                self.copy("*", dst="lib", src=os.path.join(output_dir, "libs/windows/x86_64"))
        elif self.settings.os == "Android":
            if self.settings.arch == "armv7":
                self.copy("*", dst="lib", src=os.path.join(output_dir, "libs/android/armv7"))
            elif self.settings.arch == "armv8":
                self.copy("*", dst="lib", src=os.path.join(output_dir, "libs/android/armv8"))
            elif self.settings.arch == "x86":
                self.copy("*", dst="lib", src=os.path.join(output_dir, "libs/android/x86"))
            elif self.settings.arch == "x86_64":
                self.copy("*", dst="lib", src=os.path.join(output_dir, "libs/android/x86_64"))
        elif self.settings.os == "Macos":
            self.copy("*", dst="lib", src=os.path.join(output_dir, "libs/macos/x64"))
        else:
            self.copy("*", dst="lib", src=os.path.join(output_dir, "libs/ios/arm64"))

    def package_info(self):
        self.cpp_info.libs = self.__collect_lib('lib')
        self.cpp_info.includedirs  = self.__collect_include('include')
        self.cpp_info.defines = [f'USE_{self.name}'.upper()]

    def __collect_lib(self, lib_dir):
        libs = []
        for root, dirs, files in os.walk(lib_dir):
            for file in files:
                if file.endswith(".lib"):
                    fname = os.path.splitext(file)[0]
                    libs.append(fname)
                elif file.endswith(".a"):
                    fname = os.path.splitext(file)[0]
                    if fname.startswith('lib'):
                        fname = fname[fname.find('lib') + 3:]
                    libs.append(fname)
        return libs

    def __collect_include(self, inc_dir):
        inc_dirs = ['include']
        return inc_dirs