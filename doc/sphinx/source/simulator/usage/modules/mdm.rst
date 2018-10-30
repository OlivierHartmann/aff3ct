.. _mdm-modem-parameters:

Modem parameters
----------------

.. _mdm-const-path:

``--const-path``
""""""""""""""""

   :type: FILE
   :Examples: ``--const-path example/path/to/the/right/file``


Path to the ordered modulation symbols (constellation), to use with "\\-\\-type USER".

.. _mdm-ite:

``--ite``
"""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 1
   :Examples: ``--ite 1``


Number of iteration in the demodulator.

.. _mdm-cpm-ws:

``--cpm-ws``
""""""""""""

   :type: TEXT
   :Allowed values: ``GMSK`` ``RCOS`` ``REC`` 
   :Default: GMSK
   :Examples: ``--cpm-ws GMSK``


Wave shape.

Description of the allowed values:

+----------+---------------------+
| Value    | Description         |
+==========+=====================+
| ``GMSK`` | |cpm-ws_descr_gmsk| |
+----------+---------------------+
| ``RCOS`` | |cpm-ws_descr_rcos| |
+----------+---------------------+
| ``REC``  | |cpm-ws_descr_rec|  |
+----------+---------------------+

.. |cpm-ws_descr_gmsk| replace:: TODO VALUE GMSK
.. |cpm-ws_descr_rcos| replace:: TODO VALUE RCOS
.. |cpm-ws_descr_rec| replace:: TODO VALUE REC


.. _mdm-max:

``--max``
"""""""""

   :type: TEXT
   :Allowed values: ``MAX`` ``MAXL`` ``MAXS`` ``MAXSS`` 
   :Default: MAX
   :Examples: ``--max MAX``


Select the type of the max operation to use in the demodulator.

Description of the allowed values:

+-----------+-------------------+
| Value     | Description       |
+===========+===================+
| ``MAX``   | |max_descr_max|   |
+-----------+-------------------+
| ``MAXL``  | |max_descr_maxl|  |
+-----------+-------------------+
| ``MAXS``  | |max_descr_maxs|  |
+-----------+-------------------+
| ``MAXSS`` | |max_descr_maxss| |
+-----------+-------------------+

.. |max_descr_max| replace:: TODO VALUE MAX
.. |max_descr_maxl| replace:: TODO VALUE MAXL
.. |max_descr_maxs| replace:: TODO VALUE MAXS
.. |max_descr_maxss| replace:: TODO VALUE MAXSS


.. _mdm-ups:

``--ups``
"""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 1
   :Examples: ``--ups 1``


Select the symbol sampling factor.

.. _mdm-cpm-L:

``--cpm-L``
"""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 2
   :Examples: ``--cpm-L 2``


CPM pulse width or CPM memory.

.. _mdm-no-sig2:

``--no-sig2``
"""""""""""""



Turn off the division by sigma square in the demodulator.

.. _mdm-cpm-p:

``--cpm-p``
"""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 2
   :Examples: ``--cpm-p 2``


Modulation index denominator.

.. _mdm-implem:

``--implem``
""""""""""""

   :type: TEXT
   :Allowed values: ``FAST`` ``STD`` 
   :Default: STD
   :Examples: ``--implem STD``


Select the implementation of the modem.

Description of the allowed values:

+----------+---------------------+
| Value    | Description         |
+==========+=====================+
| ``FAST`` | |implem_descr_fast| |
+----------+---------------------+
| ``STD``  | |implem_descr_std|  |
+----------+---------------------+

.. |implem_descr_fast| replace:: TODO VALUE FAST
.. |implem_descr_std| replace:: TODO VALUE STD


.. _mdm-cpm-k:

``--cpm-k``
"""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 1
   :Examples: ``--cpm-k 1``


Modulation index numerator.

.. _mdm-bps:

``--bps``
"""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 1
   :Examples: ``--bps 1``


Select the number of bits per symbol.

.. _mdm-cpm-map:

``--cpm-map``
"""""""""""""

   :type: TEXT
   :Allowed values: ``GRAY`` ``NATURAL`` 
   :Default: NATURAL
   :Examples: ``--cpm-map NATURAL``


Symbols mapping layout.

Description of the allowed values:

+-------------+-------------------------+
| Value       | Description             |
+=============+=========================+
| ``GRAY``    | |cpm-map_descr_gray|    |
+-------------+-------------------------+
| ``NATURAL`` | |cpm-map_descr_natural| |
+-------------+-------------------------+

.. |cpm-map_descr_gray| replace:: TODO VALUE GRAY
.. |cpm-map_descr_natural| replace:: TODO VALUE NATURAL


.. _mdm-cpm-std:

``--cpm-std``
"""""""""""""

   :type: TEXT
   :Allowed values: ``GSM`` 
   :Examples: ``--cpm-std GSM``


The selection of a default CPM standard hardly implemented (those parameters overwrite the other arguments given by the user).

Description of the allowed values:

+---------+---------------------+
| Value   | Description         |
+=========+=====================+
| ``GSM`` | |cpm-std_descr_gsm| |
+---------+---------------------+

.. |cpm-std_descr_gsm| replace:: TODO VALUE GSM


.. _mdm-psi:

``--psi``
"""""""""

   :type: TEXT
   :Allowed values: ``PSI0`` ``PSI1`` ``PSI2`` ``PSI3`` 
   :Default: PSI0
   :Examples: ``--psi PSI0``


Select the type of the psi function to use in the SCMA demodulator.

Description of the allowed values:

+----------+------------------+
| Value    | Description      |
+==========+==================+
| ``PSI0`` | |psi_descr_psi0| |
+----------+------------------+
| ``PSI1`` | |psi_descr_psi1| |
+----------+------------------+
| ``PSI2`` | |psi_descr_psi2| |
+----------+------------------+
| ``PSI3`` | |psi_descr_psi3| |
+----------+------------------+

.. |psi_descr_psi0| replace:: TODO VALUE PSI0
.. |psi_descr_psi1| replace:: TODO VALUE PSI1
.. |psi_descr_psi2| replace:: TODO VALUE PSI2
.. |psi_descr_psi3| replace:: TODO VALUE PSI3


.. _mdm-type:

``--type``
""""""""""

   :type: TEXT
   :Allowed values: ``BPSK`` ``CPM`` ``OOK`` ``PAM`` ``PSK`` ``QAM`` ``SCMA`` ``USER`` 
   :Default: BPSK
   :Examples: ``--type BPSK``


Type of the modulation to use in the simulation.

Description of the allowed values:

+----------+-------------------+
| Value    | Description       |
+==========+===================+
| ``BPSK`` | |type_descr_bpsk| |
+----------+-------------------+
| ``CPM``  | |type_descr_cpm|  |
+----------+-------------------+
| ``OOK``  | |type_descr_ook|  |
+----------+-------------------+
| ``PAM``  | |type_descr_pam|  |
+----------+-------------------+
| ``PSK``  | |type_descr_psk|  |
+----------+-------------------+
| ``QAM``  | |type_descr_qam|  |
+----------+-------------------+
| ``SCMA`` | |type_descr_scma| |
+----------+-------------------+
| ``USER`` | |type_descr_user| |
+----------+-------------------+

.. |type_descr_bpsk| replace:: TODO VALUE BPSK
.. |type_descr_cpm| replace:: TODO VALUE CPM
.. |type_descr_ook| replace:: TODO VALUE OOK
.. |type_descr_pam| replace:: TODO VALUE PAM
.. |type_descr_psk| replace:: TODO VALUE PSK
.. |type_descr_qam| replace:: TODO VALUE QAM
.. |type_descr_scma| replace:: TODO VALUE SCMA
.. |type_descr_user| replace:: TODO VALUE USER


