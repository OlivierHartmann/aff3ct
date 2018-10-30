.. _chn-channel-parameters:

Channel parameters
------------------

.. _chn-blk-fad:

``--blk-fad``
"""""""""""""

   :type: TEXT
   :Allowed values: ``FRAME`` ``NO`` ``ONETAP`` 
   :Default: NO
   :Group: Standard
   :Examples: ``--blk-fad NO``


Block fading policy for the RAYLEIGH channel.

Description of the allowed values:

+------------+------------------------+
| Value      | Description            |
+============+========================+
| ``FRAME``  | |blk-fad_descr_frame|  |
+------------+------------------------+
| ``NO``     | |blk-fad_descr_no|     |
+------------+------------------------+
| ``ONETAP`` | |blk-fad_descr_onetap| |
+------------+------------------------+

.. |blk-fad_descr_frame| replace:: TODO VALUE FRAME
.. |blk-fad_descr_no| replace:: TODO VALUE NO
.. |blk-fad_descr_onetap| replace:: TODO VALUE ONETAP


.. _chn-implem:

``--implem``
""""""""""""

   :type: TEXT
   :Allowed values: ``FAST`` ``STD`` 
   :Default: STD
   :Group: Standard
   :Examples: ``--implem STD``


Select the implementation of the algorithm to generate noise.

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


.. _chn-path:

``--path``
""""""""""

   :type: FILE
   :Group: Standard
   :Examples: ``--path example/path/to/the/right/file``


Path to a noisy file, to use with "\\-\\-type USER,OPTICAL" or to a gain file (used with "\\-\\-type RAYLEIGH_USER").

.. _chn-gain-occur:

``--gain-occur``
""""""""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 1
   :Group: Standard
   :Examples: ``--gain-occur 1``


The number of times a gain is used on consecutive symbols (used with "\\-\\-type RAYLEIGH_USER")

.. _chn-type:

``--type``
""""""""""

   :type: TEXT
   :Allowed values: ``AWGN`` ``BEC`` ``BSC`` ``NO`` ``OPTICAL`` ``RAYLEIGH`` ``RAYLEIGH_USER`` ``USER`` ``USER_ADD`` 
   :Default: AWGN
   :Group: Standard
   :Examples: ``--type AWGN``


Type of the channel to use in the simulation ('USER' has an output got from a file when 'USER_ADD' has an additive noise got from a file).

Description of the allowed values:

+-------------------+----------------------------+
| Value             | Description                |
+===================+============================+
| ``AWGN``          | |type_descr_awgn|          |
+-------------------+----------------------------+
| ``BEC``           | |type_descr_bec|           |
+-------------------+----------------------------+
| ``BSC``           | |type_descr_bsc|           |
+-------------------+----------------------------+
| ``NO``            | |type_descr_no|            |
+-------------------+----------------------------+
| ``OPTICAL``       | |type_descr_optical|       |
+-------------------+----------------------------+
| ``RAYLEIGH``      | |type_descr_rayleigh|      |
+-------------------+----------------------------+
| ``RAYLEIGH_USER`` | |type_descr_rayleigh_user| |
+-------------------+----------------------------+
| ``USER``          | |type_descr_user|          |
+-------------------+----------------------------+
| ``USER_ADD``      | |type_descr_user_add|      |
+-------------------+----------------------------+

.. |type_descr_awgn| replace:: TODO VALUE AWGN
.. |type_descr_bec| replace:: TODO VALUE BEC
.. |type_descr_bsc| replace:: TODO VALUE BSC
.. |type_descr_no| replace:: TODO VALUE NO
.. |type_descr_optical| replace:: TODO VALUE OPTICAL
.. |type_descr_rayleigh| replace:: TODO VALUE RAYLEIGH
.. |type_descr_rayleigh_user| replace:: TODO VALUE RAYLEIGH_USER
.. |type_descr_user| replace:: TODO VALUE USER
.. |type_descr_user_add| replace:: TODO VALUE USER_ADD


