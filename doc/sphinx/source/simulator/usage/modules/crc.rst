.. _crc-crc-parameters:

CRC parameters
--------------

.. _crc-type:

``--type,--poly``
"""""""""""""""""

   :type: TEXT
   :Default: NO
   :Examples: ``--type NO``


Select the CRC type/polynomial you want to use (ex: "8-DVB-S2": 0xD5, "16-IBM": 0x8005, "24-LTEA": 0x864CFB, "32-GZIP": 0x04C11DB7).

.. _crc-implem:

``--implem``
""""""""""""

   :type: TEXT
   :Allowed values: ``FAST`` ``INTER`` ``STD`` 
   :Default: FAST
   :Examples: ``--implem FAST``


Select the CRC implementation you want to use.

Description of the allowed values:

+-----------+----------------------+
| Value     | Description          |
+===========+======================+
| ``FAST``  | |implem_descr_fast|  |
+-----------+----------------------+
| ``INTER`` | |implem_descr_inter| |
+-----------+----------------------+
| ``STD``   | |implem_descr_std|   |
+-----------+----------------------+

.. |implem_descr_fast| replace:: TODO VALUE FAST
.. |implem_descr_inter| replace:: TODO VALUE INTER
.. |implem_descr_std| replace:: TODO VALUE STD


.. _crc-size:

``--size``
""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Examples: ``--size 1``


Size of the CRC (divisor size in bit -1), required if you selected an unknown CRC.

