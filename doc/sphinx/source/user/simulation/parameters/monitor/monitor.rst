.. _mnt-monitor-parameters:

Monitor parameters
------------------

The monitor is the last module in the chain: **it compares the decoded
information bits with the initially generated ones from the source**.
Furthermore, it can also compute **the mutual information** (|MI|) from the
demodulator output.

.. _mnt-mnt-max-fe:

``--mnt-max-fe, -e``
""""""""""""""""""""

   :Type: integer
   :Default: 100
   :Examples: ``--mnt-max-fe 25``

Set the maximum number of frame errors to simulated for each noise point.

.. _mnt-mnt-err-hist:

``--mnt-err-hist``
""""""""""""""""""

   :Type: integer
   :Examples: ``--mnt-err-hist 0``

Enable the construction of the errors per frame histogram. Set also the maximum
number of bit errors per frame included in the histogram (0 means no limit).
The histogram is saved in |CSV| format:

.. code-block:: bash

   "Number of error bits per wrong frame"; "Histogram (noise: 5.000000dB, on 10004 frames)"
   0; 0
   1; 7255
   2; 2199
   3; 454
   4; 84
   5; 11
   6; 12

.. _mnt-mnt-err-hist-path:

``--mnt-err-hist-path``
"""""""""""""""""""""""

   :Type: file
   :Rights: write only
   :Default: :file:`./hist`
   :Examples: ``--mnt-err-hist-path my/histogram/root/path/name``

Path to the output histogram. When the files are dumped, the current noise value
is added to this name with the ``.txt`` extension.

An output filename example is :file:`hist_2.000000.txt` for a noise value of
2 dB. For `Gnuplot <http://www.gnuplot.info/>`_ users you can then simply
display the histogram with the following command:

.. code-block:: bash

   gnuplot -e "set key autotitle columnhead; plot 'hist_2.000000.txt' with lines; pause -1"

.. _mnt-mnt-mutinfo:

``--mnt-mutinfo``
"""""""""""""""""

Enable the computation of the |MI|.

.. note:: Only available on ``BFER`` simulation types (see the
   :ref:`sim-sim-type` parameter for more details).