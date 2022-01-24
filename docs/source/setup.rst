Installing `cliquematch`
==========================

`cliquematch` uses |pybind11|_ to provide Python wrappers. Internally, the core
clique search algorithm is implemented in ``C++11``, simple template classes
are used to provide flexibility for applications, and |Eigen|_ is used to
provide fast access to numpy arrays.

Installing from a wheel
-----------------------

`PyPI`_ wheels are available for Linux and Windows.  

.. code:: bash

    pip install cliquematch>=3.0.0

Installing from source
----------------------

1. `cliquematch` requires |pybind11|_ (v2.2 or newer) for its setup:

.. code:: bash

    pip3 install pybind11

2. `cliquematch` requires |Eigen|_ (v3.3.7 or newer) as part of its setup.

   -  You can clone the `Github repo`_ via ``git clone --recursive`` to get |Eigen|_.  
   - If you already have an existing version of |Eigen|_ or have downloaded it
     separately, set the ``EIGEN_DIR`` environment variable to the folder
     containing |Eigen|_ before compilation.

3. A ``C++11`` compatible compiler must be available for the
   installation:

   -  On Linux, gcc is called with ``--std=c++11`` (builds with
      ``gcc 4.8.2`` for ``manylinux1`` wheels).
   -  On Windows, Visual Studio 2015 Update 3 (MSVC 14.0 runtime) or
      later is needed.
   -  **Note:** Installing under Windows+MinGW has not been tested.


.. |pybind11| replace:: ``pybind11``
.. _pybind11: https://github.com/pybind/pybind11/
.. |Eigen| replace:: ``Eigen`` 
.. _Eigen: https://gitlab.com/libeigen/eigen/-/releases#3.3.7
.. _PyPI: https://pypi.org/project/cliquematch/
.. _Github repo: https://github.com/ahgamut/cliquematch
