#!/usr/bin/env bash
echo "travis tag was originally $TRAVIS_TAG"
TRAVIS_TAG=$(git tag -l --contains HEAD)
echo "git cmd gave $TRAVIS_TAG"
python3 -m pip install importlib-metadata>=1
python3 -m pip install twine
if [[ -z "$TRAVIS_TAG" ]]; then
	echo "No tag; not uploading to PyPI"
else
	echo "Uploading $TRAVIS_TAG to PYPI via twine..."
	python3 -m twine check wheelhouse/*.whl
	python3 -m twine upload --skip-existing wheelhouse/*.whl
fi
