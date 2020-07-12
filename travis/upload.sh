#!/usr/bin/env bash

if [[ -z "$TRAVIS_TAG" ]]; then
	echo "No tag; not uploading to PyPI"
else
	echo "Uploading $TRAVIS_TAG to PYPI via twine..."
	python3 -m pip install twine
	python3 -m twine check wheelhouse/*.whl
	python3 -m twine upload --skip-existing wheelhouse/*.whl
fi
