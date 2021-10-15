from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext as _build_ext
import sys
import os
import platform


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked."""

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11

        return pybind11.get_include(self.user)


def get_src_cpps(path):
    """
    Go through src/cliquematch/cm_base recursively,
    and return all cpp file paths
    """
    cpps = []
    for root, dirnames, filenames in os.walk(path):
        for filename in filenames:
            if os.path.splitext(filename)[-1] == ".cpp":
                cpps.append(os.path.join(root, filename))
    return cpps


# extension naming matters, because
# 1) the .DEF file used by the linker
# 2) the copying from build/ to dist/
ext_modules = [
    Extension(
        name="cliquematch.core",
        sources=get_src_cpps("src/cliquematch/detail/")
        + get_src_cpps("src/cliquematch/core/")
        + get_src_cpps("src/cliquematch/ext/"),
        include_dirs=[
            str(get_pybind_include(True)),
            str(get_pybind_include(False)),
            "src/cliquematch/",
            os.environ.get("EIGEN_DIR", "include/"),
        ],
        language="c++",
    )
]


def has_flag(compiler, flagname):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile

    with tempfile.NamedTemporaryFile("w", suffix=".cpp") as f:
        print("Testing for flag %s" % (flagname))
        f.write("int main (int argc, char **argv) { return 0; }")
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except Exception as e:
            print(e)
            return False
    return True


class BuildExt(_build_ext):
    """A custom build extension for adding compiler-specific options."""

    c_opts = {
        "msvc": ["/EHsc"],  # msvc has c++11 by default
        "unix": ["-Wall", "-Wpedantic", "-Wno-unused-result", "-std=c++11"],
    }

    if platform.system() == "Windows":
        os.environ["CC"] = "msvc"
        os.environ["CXX"] = "msvc"

    if sys.platform == "darwin":
        c_opts["unix"] += ["-stdlib=libc++", "-mmacosx-version-min=10.7"]

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct)
        if ct == "unix":
            if has_flag(self.compiler, "-fvisibility=hidden"):
                opts.append("-fvisibility=hidden")
            if self.compiler.compiler_so:
                if "-g" in self.compiler.compiler_so:
                    self.compiler.compiler_so.remove("-g")
        elif ct == "mingw32":
            opts = self.c_opts.get("unix")

        eigen_dir = os.environ.get("EIGEN_DIR", "include")
        sample_file = os.path.abspath(os.path.join(eigen_dir, "Eigen", "Dense"))
        print("Checking for the Eigen Matrix Library at %s ..." % (sample_file))
        if os.path.exists(sample_file):
            print("File exists! Assuming the Eigen Matrix Library is available")
        else:
            err_str = (
                "Could not find Eigen. Please download Eigen from:\n"
                + "\t https://gitlab.com/libeigen/eigen/-/releases#3.3.7\n"
                + "extract into a folder, and set the EIGEN_DIR environment variable accordingly"
            )
            print(err_str)
            exit(1)

        for ext in self.extensions:
            ext.extra_compile_args = opts
        _build_ext.build_extensions(self)


setup(
    name="cliquematch",
    version="2.2.1",
    author="Gautham Venkatasubramanian",
    author_email="ahgamut@gmail.com",
    description="Finding correspondence via maximum cliques in large graphs",
    license="MIT",
    long_description=open("README.md", "r").read(),
    long_description_content_type="text/markdown",
    url="https://github.com/ahgamut/cliquematch",
    packages=find_packages("src"),
    package_dir={"": "src"},
    classifiers=[
        "Intended Audience :: Science/Research",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
    ],
    zip_safe=False,
    install_requires=["pybind11>=2.2", "numpy>=1.14"],
    setup_requires=["pybind11>=2.2"],
    ext_modules=ext_modules,
    cmdclass={"build_ext": BuildExt},
)
