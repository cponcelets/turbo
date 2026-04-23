# Available at setup time due to pyproject.toml
import os
import re
import subprocess
import sys
import shutil
from pathlib import Path

from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext

# Convert distutils Windows platform specifiers to CMake -A arguments
PLAT_TO_CMAKE = {
    "win32": "Win32",
    "win-amd64": "x64",
    "win-arm32": "ARM",
    "win-arm64": "ARM64",
}


# A CMakeExtension needs a sourcedir instead of a file list.
# The name must be the _single_ output extension from the CMake build.
# If you need multiple extensions, see scikit-build.
class CMakeExtension(Extension):
    def __init__(self, name: str, sourcedir: str = "") -> None:
        super().__init__(name, sources=[])
        self.sourcedir = os.fspath(Path(sourcedir).resolve())


class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext: CMakeExtension) -> None:
        # Take the preset from command line, cpu-release-local by default (need to cline lala-land before)
        preset = os.environ.get("CMAKE_PRESET", "cpu-release-local")

        # Allow override from pip config-settings
        if hasattr(self, "distribution"):
            preset = self.distribution.get_option_dict("build_ext").get("cmake.preset", (None, preset))[1]

        # Must be in this form due to bug in .resolve() only fixed in Python 3.10+
        ext_fullpath = Path.cwd() / self.get_ext_fullpath(ext.name)
        extdir = ext_fullpath.parent.resolve()
        # Using this requires trailing slash for auto-detection & inclusion of
        # auxiliary "native" libs

        debug = int(os.environ.get("DEBUG", 0)) if self.debug is None else self.debug
        cfg = "Debug" if debug else "Release"

	# Run your preset workflow instead of manual configure
        subprocess.run(["cmake", "--workflow", "--preset", preset, "--fresh"],
        cwd=ext.sourcedir,
        check=True
        )

	# Then build with preset
        subprocess.run(["cmake", "--build", "--preset", preset],
        cwd=ext.sourcedir,
    	check=True
	)

        so_files = list(Path(ext.sourcedir, "build", "cpu-release-local").glob("turbo_python*.so"))
        if not so_files:
            raise RuntimeError("No turbo_python .so found in after build")

        so_name = so_files[0]
        print(f"Installing {so_name} into {extdir}")
        shutil.copy2(so_name, extdir)

# The information here can also be placed in setup.cfg - better separation of
# logic and declaration, and simpler if you include description/version in a file.

setup(
    name="turbo_python",
    version="0.0.1",
    author="Clement Poncelet Sanchez",
    author_email="clement.poncelet@uni.lu",
    url="https://github.com/ptal/turbo/tree/pythonAPI",
    description="A turbo project using pybind11",
    long_description="",
    ext_modules=[CMakeExtension("turbo_python")],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    extras_require={"test": ["pytest>=6.0"]},
    python_requires=">=3.9",
    package_data={"turbo_python": ["*.so"]},
    include_package_data=True,
)
