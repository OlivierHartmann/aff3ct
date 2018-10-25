On Debian-like OS
-----------------

Install cmake and clone AFF3CT
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Open a shell and type:

.. code-block:: bash

   sudo apt-get install git cmake
   git clone https://github.com/aff3ct/aff3ct.git
   cd aff3ct
   git submodule update --init --recursive


Makefile project generation (with the C++ GNU compiler)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Open a shell and type (from the AFF3CT root folder):

.. code-block:: bash

   mkdir build
   cd build
   cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-funroll-loops -march=native -DMULTI_PREC -DENABLE_COOL_BASH"


Compile the code
^^^^^^^^^^^^^^^^

This command will use the generated Makefile to compile::

.. code-block:: bash

   make -j4

