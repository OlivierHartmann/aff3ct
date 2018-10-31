.. _src-source-parameters:

Source parameters
-----------------

.. _src-fra:

``-F,--fra``
""""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 1
   :Examples: ``--fra 1``


Set the number of inter frame level to process.

.. _src-implem:

``--implem``
""""""""""""

   :type: TEXT
   :Allowed values: ``FAST`` ``STD`` 
   :Default: STD
   :Examples: ``--implem STD``


Select the implementation of the algorithm to generate the information bits.

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


.. _src-path:

``--path``
""""""""""

   :type: FILE
   :Examples: ``--path example/path/to/the/right/file``


Path to a file containing one or a set of pre-computed source bits (to use with "\\-\\-type USER").

.. _src-start-idx:

``--start-idx``
"""""""""""""""

   :type: UINT
   :Default: 0
   :Examples: ``--start-idx 0``


Start idx to use in the USER type source.

.. _src-type:

``--type``
""""""""""

   :type: TEXT
   :Allowed values: ``AZCW`` ``RAND`` ``USER`` 
   :Default: RAND
   :Examples: ``--type RAND``


Method used to generate the codewords.

Description of the allowed values:

+----------+-------------------+
| Value    | Description       |
+==========+===================+
| ``AZCW`` | |type_descr_azcw| |
+----------+-------------------+
| ``RAND`` | |type_descr_rand| |
+----------+-------------------+
| ``USER`` | |type_descr_user| |
+----------+-------------------+

.. |type_descr_azcw| replace:: TODO VALUE AZCW
.. |type_descr_rand| replace:: TODO VALUE RAND
.. |type_descr_user| replace:: TODO VALUE USER


