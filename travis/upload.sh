#!/usr/bin/env bash

echo "Uploading to test-PYPI via twine..."
# add a condition to upload only on tags
python3 -m pip install twine
python3 -m twine check wheelhouse/*.whl
python3 -m twine upload --repository testpypi --skip-existing wheelhouse/*.whl
