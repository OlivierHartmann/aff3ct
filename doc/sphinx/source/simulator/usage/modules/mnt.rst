.. _mnt-monitor-parameters:

Monitor parameters
------------------

.. _mnt-max-fe:

``-e,--max-fe``
"""""""""""""""

   :type: UINT
   :Range: :math:`]0 - \infty[`
   :Default: 100
   :Examples: ``--max-fe 100``


Max number of frame errors for each noise point simulation.

.. _mnt-mutinfo:

``--mutinfo``
"""""""""""""



Allow the computation of the mutual information.

.. _mnt-err-hist-path:

``--err-hist-path``
"""""""""""""""""""

   :type: TEXT
   :Default: hist
   :Examples: ``--err-hist-path hist``


Path to the output histogram (add automatically the current noise value and the extension '.txt').

.. _mnt-err-hist:

``--err-hist``
""""""""""""""

   .. image:: https://comps.canstockphoto.com/advanced-stamp-sign-stock-illustration_csp42774668.jpg
      :width:  80px
      :height: 80px
      :align: right

   :type: INT
   :Range: :math:`[0 - \infty[`
   :Examples: ``--err-hist 1``


Activate the histogram of the number of errors per frame. Set the max number of bit error per frame included in the histogram (0 is no limit). Subcommands:

