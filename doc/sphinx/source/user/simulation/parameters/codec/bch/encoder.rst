.. _enc-bch-encoder-parameters:

|BCH| Encoder parameters
------------------------

.. _enc-bch-enc-cw-size:

``--enc-cw-size, -N`` |image_required_argument|
"""""""""""""""""""""""""""""""""""""""""""""""

   :Type: integer
   :Examples: ``--enc-cw-size 127``

Set the codeword size as an integer of the form :math:`N = 2^m – 1`, where
:math:`m` is an integer from 3.

.. _enc-bch-enc-info-bits:

``--enc-info-bits, -K`` |image_required_argument|
"""""""""""""""""""""""""""""""""""""""""""""""""

   :Type: integer
   :Examples: ``--enc-info-bits 92``

Give the useful number of bit transmitted (information bits).

This argument is not required if :ref:`dec-bch-dec-corr-pow` is given, as it
is calculated automatically.

.. _enc-bch-enc-type:

``--enc-type``
""""""""""""""

   :Type: text
   :Allowed values: ``BCH`` ``AZCW`` ``COSET`` ``USER``
   :Default: ``BCH``
   :Examples: ``--enc-type AZCW``

Select the type of the encoder to use in the simulation.

Description of the allowed values:

+-----------+------------------------+
| Value     | Description            |
+===========+========================+
| ``BCH``   | |enc-type_descr_bch|   |
+-----------+------------------------+
| ``AZCW``  | |enc-type_descr_azcw|  |
+-----------+------------------------+
| ``COSET`` | |enc-type_descr_coset| |
+-----------+------------------------+
| ``USER``  | |enc-type_descr_user|  |
+-----------+------------------------+

.. |enc-type_descr_bch| replace:: Select the standard |BCH| encoder.
.. |enc-type_descr_azcw| replace:: See the common :ref:`enc-common-enc-type`
   parameter.
.. |enc-type_descr_coset| replace:: See the common :ref:`enc-common-enc-type`
   parameter.
.. |enc-type_descr_user| replace:: See the common :ref:`enc-common-enc-type`
   parameter.