Simulation BFER standard parameters
-----------------------------------

``--max-frame`` + *UINT*

   :description: Maximum number of frames to play after what the current simulatated noise stops (0 is infinite).



``--err-trk``

   :description: Enable the tracking of the bad frames (by default the frames are stored in the current folder).



``-d,--debug-limit`` + *UINT*

   :description: Enable debug mode: set the max number of elements to display in the debug mode (0 is all).
   :restrictions: in ]0 - inf[



``-p,--prec`` + *UINT*

   :description: The simulation precision in bits.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `8`
         * `16`
         * `32`
         * `64`




``-R,--noise-range`` + *MATLAB VECTOR STYLE*

   :description: Noise energy range to run (Matlab style: "0.5:2.5,2.55,2.6:0.05:3" with a default step of 0.1).



``--stop-time`` + *TIME [SEC]*

   :description: Time in sec after what the current simulatated noise stops (0 is infinite).



``--type`` + *TEXT*

   :description: Select the type of simulation to launch (default is BFER).
   :restrictions: 

      .. hlist::
         :columns: 3

         * `BFER`
         * `BFERI`
         * `EXIT`




``-m,--noise-min`` + *FLOAT*

   :description: Minimal noise energy to simulate.



``--err-trk-rev``

   :description: Automatically replay the saved frames.



``--crit-nostop``

   :description: The stop criteria arguments -stop-time or -max-frame kill the current simulatated noise point but not the simulation.



``-C,--cde-type`` + *TEXT*

   :description: Select the code type you want to use.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `BCH`
         * `LDPC`
         * `POLAR`
         * `RA`
         * `REP`
         * `RS`
         * `RSC`
         * `RSC_DB`
         * `TURBO`
         * `TURBO_DB`
         * `TURBO_PROD`
         * `UNCODED`




``--debug``

   :description: Enable debug mode: print array values after each step.



``--coded``

   :description: Enable the coded monitoring (extends the monitored bits to the entire codeword).



``-M,--noise-max`` + *FLOAT*

   :description: Maximal noise energy to simulate.



``-S,--seed`` + *UINT*

   :description: 0            Seed used in the simulation to initialize the pseudo random generators in general.



``-s,--noise-step`` + *FLOAT*

   :description: Noise energy step between each simulation iteration.
   :restrictions: in ]0 - inf[



``--meta`` + *TEXT*

   :description: Print the output with metadata, takes the simulation title.



``-t,--threads`` + *UINT*

   :description: 0         Enable multi-threaded mode and specify the number of threads (0 means the maximum supported by the core).



``--err-trk-thold`` + *UINT*

   :description: Dump only frames with a bit error count above or equal to this threshold.
   :restrictions: in ]0 - inf[



``--debug-prec`` + *INT*

   :description: Set the precision of real elements when displayed in debug mode.
   :restrictions: in [0 - 10]



``--debug-hex``

   :description: Enable debug mode: prints values in the hexadecimal format.



``--pdf-path`` + *FILE*

   :description: A file that contains PDF for different SNR. Set the SNR range from the given ones. Overwritten by -R or limited by -m and -M with a minimum step of -s



``-E,--noise-type`` + *TEXT*

   :description: Select the type of NOISE: SNR per Symbol / SNR per information Bit / Received Optical Power / Erasure Probability.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `EBN0`
         * `EP`
         * `ESN0`
         * `ROP`




``-c,--coset``

   :description: Enable the coset approach.



``--err-trk-path`` + *TEXT*

   :description: Base path for the files where the bad frames will be stored or read.



``--stats``

   :description: Display statistics module by module.



