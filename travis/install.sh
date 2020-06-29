#!/usr/bin/env bash
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	echo "On Linux, pulling docker image $DOCKER_IMAGE"
	docker pull "$DOCKER_IMAGE"
else
	echo "On MacOS, installing dependencies + pytest"
	python3 -m pip install numpy>=1.14 --prefer-binary
	python3 -m pip install pytest pybind11>=2.2
	python3 setup.py install
fi

