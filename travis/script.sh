#!/usr/bin/env bash
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	echo "On Linux, starting docker image"
	docker run --rm -e PLAT="$PLAT" -v `pwd`:/io/ "$DOCKER_IMAGE" $PRE_CMD /io/travis/build_wheels.sh
else
	echo "On MacOS, using system python for installing package"
	pytest
	python3 setup.py bdist_wheel
	python3 -m pip install -q delocate
	# should run delocate here to fix dependencies, in case I want to supply MacOS wheels
	# delocate-listdeps --depending dist/cliquematch*.whl
	# delocate-wheel -w wheelhouse --require-archs=intel dist/cliquematch*.whl
	ls dist/ -al --block-size=K
fi

