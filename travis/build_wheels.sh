#!/usr/bin/env bash
# Example taken from https://github.com/pypa/python-manylinux-demo
set -e -u

function repair_wheel {
    wheel="$1"
    if ! auditwheel show "$wheel"; then
        echo "Skipping non-platform wheel $wheel"
    else
        auditwheel repair "$wheel" --plat "$PLAT" -w /io/wheelhouse/
    fi
}

function is_pyver_3 {
	PYVER=$("$1/python" -c 'import sys; print(sys.version_info.major)')
	if [[ $PYVER == 2 ]]; then
		echo "$1 is not supported."
		true
	fi
	false
}

which gcc
gcc --version

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" install -q numpy>=1.14 --prefer-binary
    "${PYBIN}/pip" install -q pybind11>=2.2
    "${PYBIN}/pip" wheel /io/ --no-deps -w wheelhouse/
done

# Bundle external shared libraries into the wheels
for whl in wheelhouse/*.whl; do
    repair_wheel "$whl"
done

# Install packages and test
for PYBIN in /opt/python/*/bin/; do
	echo $(is_pyver_3 "${PYBIN}")
    "${PYBIN}/pip" install cliquematch --no-index -f /io/wheelhouse
    "${PYBIN}/pip" install -q pytest --prefer-binary
	cd /io/
	"${PYBIN}/pytest"
done
