.. _sim-simulation-bfer-iterative-parameters:

Simulation BFER iterative parameters
------------------------------------

.. _sim-cde-type:

``-C,--cde-type`` |image_required_argument|
"""""""""""""""""""""""""""""""""""""""""""

   :type: TEXT
   :Allowed values: ``BCH`` ``LDPC`` ``POLAR`` ``RA`` ``REP`` ``RS`` ``RSC`` ``RSC_DB`` ``TURBO`` ``TURBO_DB`` ``TURBO_PROD`` ``UNCODED`` 
   :Examples: ``--cde-type BCH``

Select the code type you want to use.

Description of the allowed values:

+----------------+-----------------------------+
| Value          | Description                 |
+================+=============================+
| ``BCH``        | |cde-type_descr_bch|        |
+----------------+-----------------------------+
| ``LDPC``       | |cde-type_descr_ldpc|       |
+----------------+-----------------------------+
| ``POLAR``      | |cde-type_descr_polar|      |
+----------------+-----------------------------+
| ``RA``         | |cde-type_descr_ra|         |
+----------------+-----------------------------+
| ``REP``        | |cde-type_descr_rep|        |
+----------------+-----------------------------+
| ``RS``         | |cde-type_descr_rs|         |
+----------------+-----------------------------+
| ``RSC``        | |cde-type_descr_rsc|        |
+----------------+-----------------------------+
| ``RSC_DB``     | |cde-type_descr_rsc_db|     |
+----------------+-----------------------------+
| ``TURBO``      | |cde-type_descr_turbo|      |
+----------------+-----------------------------+
| ``TURBO_DB``   | |cde-type_descr_turbo_db|   |
+----------------+-----------------------------+
| ``TURBO_PROD`` | |cde-type_descr_turbo_prod| |
+----------------+-----------------------------+
| ``UNCODED``    | |cde-type_descr_uncoded|    |
+----------------+-----------------------------+

.. |cde-type_descr_bch| replace:: TODO VALUE BCH
.. |cde-type_descr_ldpc| replace:: TODO VALUE LDPC
.. |cde-type_descr_polar| replace:: TODO VALUE POLAR
.. |cde-type_descr_ra| replace:: TODO VALUE RA
.. |cde-type_descr_rep| replace:: TODO VALUE REP
.. |cde-type_descr_rs| replace:: TODO VALUE RS
.. |cde-type_descr_rsc| replace:: TODO VALUE RSC
.. |cde-type_descr_rsc_db| replace:: TODO VALUE RSC_DB
.. |cde-type_descr_turbo| replace:: TODO VALUE TURBO
.. |cde-type_descr_turbo_db| replace:: TODO VALUE TURBO_DB
.. |cde-type_descr_turbo_prod| replace:: TODO VALUE TURBO_PROD
.. |cde-type_descr_uncoded| replace:: TODO VALUE UNCODED


.. _sim-coset:

``-c,--coset``
""""""""""""""


Enable the coset approach.

.. _sim-coded:

``--coded``
"""""""""""


Enable the coded monitoring (extends the monitored bits to the entire codeword).

.. _sim-crc-start:

``--crc-start``
"""""""""""""""

   :type: UINT
   :Default: 2
   :Examples: ``--crc-start 2``

Iteration number to start the CRC checking in the turbo demodulation process.

.. _sim-debug-limit:

``-d,--debug-limit``
""""""""""""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Examples: ``--debug-limit 1``

Enable debug mode: set the max number of elements to display in the debug mode (0 is all).

.. _sim-debug:

``--debug``
"""""""""""


Enable debug mode: print array values after each step.

.. _sim-debug-hex:

``--debug-hex``
"""""""""""""""


Enable debug mode: prints values in the hexadecimal format.

.. _sim-debug-prec:

``--debug-prec``
""""""""""""""""

   :type: INT
   :Range: :math:`[0 - 10]`
   :Default: 2
   :Examples: ``--debug-prec 2``

Set the precision of real elements when displayed in debug mode.

.. _sim-noise-type:

``-E,--noise-type``
"""""""""""""""""""

   :type: TEXT
   :Allowed values: ``EBN0`` ``EP`` ``ESN0`` ``ROP`` 
   :Default: EBN0
   :Examples: ``--noise-type EBN0``

Select the type of NOISE: SNR per Symbol / SNR per information Bit / Received Optical Power / Erasure Probability.

Description of the allowed values:

+----------+-------------------------+
| Value    | Description             |
+==========+=========================+
| ``EBN0`` | |noise-type_descr_ebn0| |
+----------+-------------------------+
| ``EP``   | |noise-type_descr_ep|   |
+----------+-------------------------+
| ``ESN0`` | |noise-type_descr_esn0| |
+----------+-------------------------+
| ``ROP``  | |noise-type_descr_rop|  |
+----------+-------------------------+

.. |noise-type_descr_ebn0| replace:: TODO VALUE EBN0
.. |noise-type_descr_ep| replace:: TODO VALUE EP
.. |noise-type_descr_esn0| replace:: TODO VALUE ESN0
.. |noise-type_descr_rop| replace:: TODO VALUE ROP


.. _sim-ite:

``-I,--ite``
""""""""""""

   :type: UINT
   :Default: 15
   :Examples: ``--ite 15``

Number of global iterations between the demodulator and the decoder.

.. _sim-noise-max:

``-M,--noise-max``
""""""""""""""""""

   :type: FLOAT
   :Excludes: ``--noise-range`` 
   :Examples: ``--noise-max 1.0``

Maximal noise energy to simulate.

.. _sim-noise-min:

``-m,--noise-min``
""""""""""""""""""

   :type: FLOAT
   :Excludes: ``--noise-range`` 
   :Examples: ``--noise-min 1.0``

Minimal noise energy to simulate.

.. _sim-meta:

``--meta``
""""""""""

   :type: TEXT
   :Examples: ``--meta "TODO CHECK VALUE"``

Print the output with metadata, takes the simulation title.

.. _sim-prec:

``-p,--prec``
"""""""""""""

   :type: UINT
   :Allowed values: ``8`` ``16`` ``32`` ``64`` 
   :Default: 32
   :Examples: ``--prec 32``

The simulation precision in bits.

Description of the allowed values:

+--------+-----------------+
| Value  | Description     |
+========+=================+
| ``8``  | |prec_descr_8|  |
+--------+-----------------+
| ``16`` | |prec_descr_16| |
+--------+-----------------+
| ``32`` | |prec_descr_32| |
+--------+-----------------+
| ``64`` | |prec_descr_64| |
+--------+-----------------+

.. |prec_descr_8| replace:: TODO VALUE 8
.. |prec_descr_16| replace:: TODO VALUE 16
.. |prec_descr_32| replace:: TODO VALUE 32
.. |prec_descr_64| replace:: TODO VALUE 64


.. _sim-pdf-path:

``--pdf-path``
""""""""""""""

   :type: FILE
   :Examples: ``--pdf-path example/path/to/the/right/file``

A file that contains PDF for different SNR. Set the SNR range from the given ones. Overwritten by -R or limited by -m and -M with a minimum step of -s

.. _sim-noise-range:

``-R,--noise-range``
""""""""""""""""""""

   :type: MATLAB VECTOR STYLE
   :Excludes: ``--noise-min`` ``--noise-max`` ``--noise-step`` 
   :Examples: ``--noise-range "TODO CHECK VALUE"``

Noise energy range to run (Matlab style: "0.5:2.5,2.55,2.6:0.05:3" with a default step of 0.1).

.. _sim-noise-step:

``-s,--noise-step``
"""""""""""""""""""

   :type: FLOAT
   :Range: :math:`]0 - \infty[`
   :Default: 0.1
   :Excludes: ``--noise-range`` 
   :Examples: ``--noise-step 0.1``

Noise energy step between each simulation iteration.

.. _sim-seed:

``-S,--seed``
"""""""""""""

   :type: UINT
   :Default: 0
   :Examples: ``--seed 0``

Seed used in the simulation to initialize the pseudo random generators in general.

.. _sim-stats:

``--stats``
"""""""""""


Display statistics module by module.

.. _sim-threads:

``-t,--threads``
""""""""""""""""

   :type: UINT
   :Default: 0
   :Examples: ``--threads 0``

Enable multi-threaded mode and specify the number of threads (0 means the maximum supported by the core).

.. _sim-type:

``--type``
""""""""""

   :type: TEXT
   :Allowed values: ``BFER`` ``BFERI`` ``EXIT`` 
   :Default: BFERI
   :Examples: ``--type BFERI``

Select the type of simulation to launch (default is BFER).

Description of the allowed values:

+-----------+--------------------+
| Value     | Description        |
+===========+====================+
| ``BFER``  | |type_descr_bfer|  |
+-----------+--------------------+
| ``BFERI`` | |type_descr_bferi| |
+-----------+--------------------+
| ``EXIT``  | |type_descr_exit|  |
+-----------+--------------------+

.. |type_descr_bfer| replace:: TODO VALUE BFER
.. |type_descr_bferi| replace:: TODO VALUE BFERI
.. |type_descr_exit| replace:: TODO VALUE EXIT


.. _sim-crit-nostop:

``--crit-nostop`` |image_advanced_argument|
"""""""""""""""""""""""""""""""""""""""""""


The stop criteria arguments -stop-time or -max-frame kill the current simulatated noise point but not the simulation.

.. _sim-err-trk:

``--err-trk`` |image_advanced_argument|
"""""""""""""""""""""""""""""""""""""""


Enable the tracking of the bad frames (by default the frames are stored in the current folder).

.. _sim-err-trk-path:

``--err-trk-path`` |image_advanced_argument|
""""""""""""""""""""""""""""""""""""""""""""

   :type: TEXT
   :Needs: ``--err-trk`` 
   :Examples: ``--err-trk-path "TODO CHECK VALUE"``

Base path for the files where the bad frames will be stored or read.

.. _sim-err-trk-rev:

``--err-trk-rev`` |image_advanced_argument|
"""""""""""""""""""""""""""""""""""""""""""


Automatically replay the saved frames.

.. _sim-err-trk-thold:

``--err-trk-thold`` |image_advanced_argument|
"""""""""""""""""""""""""""""""""""""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Needs: ``--err-trk`` 
   :Examples: ``--err-trk-thold 1``

Dump only frames with a bit error count above or equal to this threshold.

.. _sim-max-frame:

``--max-frame`` |image_advanced_argument|
"""""""""""""""""""""""""""""""""""""""""

   :type: UINT
   :Examples: ``--max-frame 1``

Maximum number of frames to play after what the current simulatated noise stops (0 is infinite).

.. _sim-stop-time:

``--stop-time`` |image_advanced_argument|
"""""""""""""""""""""""""""""""""""""""""

   :type: TIME [SEC]
   :Examples: ``--stop-time 10``

Time in sec after what the current simulatated noise stops (0 is infinite).

