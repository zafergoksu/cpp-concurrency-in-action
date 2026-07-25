from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CppConcurrencyInActionConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.test_requires("gtest/1.17.0")

    def layout(self):
        cmake_layout(self)
