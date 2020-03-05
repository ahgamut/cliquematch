import setuptools
from setuptools import setup, find_packages, Extension
from setuptools.command.build_ext import build_ext as _build_ext
from distutils.command.build import build as _build
import sys
import os
import platform
import glob
import distutils.log

distutils.log.set_verbosity(distutils.log.DEBUG)


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

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
        sources=get_src_cpps("src/cliquematch/core/")
        + get_src_cpps("src/cliquematch/ext/"),
        include_dirs=[
            str(get_pybind_include(True)),
            str(get_pybind_include(False)),
            "src/cliquematch/",
            os.environ.get("EIGEN_DIR", "include/"),
        ],
        define_macros=[("WRAPPY", "1"), ("WRAPR", "0"), ("STACK_DFS", "")],
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
        except setuptools.distutils.errors.CompileError:
            return False
    return True


class BuildExt(_build_ext):
    """A custom build extension for adding compiler-specific options."""

    c_opts = {
        "msvc": ["/EHsc", "-std=c++14"],  # msvc has c++11 by default
        "unix": ["-Wall", "-Wno-unused-result", "-std=c++14"],
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
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            if has_flag(self.compiler, "-fvisibility=hidden"):
                opts.append("-fvisibility=hidden")

        elif ct == "msvc":
            opts.append('-DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        elif ct == "mingw32":
            print(
                "\n\nUsing MINGW, including static versions of libgcc, libstdc++, and winpthread\n\n"
            )
            opts = self.c_opts.get("unix")

        for ext in self.extensions:
            ext.extra_compile_args = opts
            if "mingw" in ct:
                ext.extra_link_args = [
                    "-Wl,-Bstatic,--whole-archive",
                    "-lwinpthread",
                    "-Wl,--no-whole-archive",
                    "-static-libgcc",
                    "-static-libstdc++",
                ]
        _build_ext.build_extensions(self)


setup(
    name="cliquematch",
    version="0.7.0",
    author="Gautham Venkatasubramanian",
    author_email="ahgamut@gmail.com",
    description="Matching using cliques in large sparse graphs",
    license="GPLv3",
    long_description=open("README.md", "r").read(),
    url="https://github.com/ahgamut/cliquematch",
    packages=find_packages("src"),
    package_dir={"": "src"},
    classifiers=[
        "Intended Audience :: Science/Research",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 2",
    ],
    zip_safe=False,
    install_requires=["pybind11>=2.2", "numpy>=1.11"],
    setup_requires=["pybind11>=2.2"],
    ext_modules=ext_modules,
    cmdclass={"build_ext": BuildExt},
)
