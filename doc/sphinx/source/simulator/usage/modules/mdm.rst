Modem parameters
----------------

``--bps`` + *UINT*

   :description: Select the number of bits per symbol.
   :restrictions: in ]0 - inf[



``--ups`` + *UINT*

   :description: Select the symbol sampling factor.
   :restrictions: in ]0 - inf[



``--cpm-ws`` + *TEXT*

   :description: Wave shape.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `GMSK`
         * `RCOS`
         * `REC`




``--implem`` + *TEXT*

   :description: Select the implementation of the modem.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `FAST`
         * `STD`




``--ite`` + *UINT*

   :description: Number of iteration in the demodulator.
   :restrictions: in ]0 - inf[



``--psi`` + *TEXT*

   :description: Select the type of the psi function to use in the SCMA demodulator.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `PSI0`
         * `PSI1`
         * `PSI2`
         * `PSI3`




``--cpm-map`` + *TEXT*

   :description: Symbols mapping layout.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `GRAY`
         * `NATURAL`




``--cpm-std`` + *TEXT*

   :description: The selection of a default CPM standard hardly implemented (those parameters overwrite the other arguments given by the user).
   :restrictions: 

      .. hlist::
         :columns: 3

         * `GSM`




``--const-path`` + *FILE*

   :description: Path to the ordered modulation symbols (constellation), to use with "\\-\\-type USER".



``--cpm-p`` + *UINT*

   :description: Modulation index denominator.
   :restrictions: in ]0 - inf[



``--no-sig2``

   :description: Turn off the division by sigma square in the demodulator.



``--cpm-k`` + *UINT*

   :description: Modulation index numerator.
   :restrictions: in ]0 - inf[



``--type`` + *TEXT*

   :description: Type of the modulation to use in the simulation.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `BPSK`
         * `CPM`
         * `OOK`
         * `PAM`
         * `PSK`
         * `QAM`
         * `SCMA`
         * `USER`




``--max`` + *TEXT*

   :description: Select the type of the max operation to use in the demodulator.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `MAX`
         * `MAXL`
         * `MAXS`
         * `MAXSS`




``--cpm-L`` + *UINT*

   :description: CPM pulse width or CPM memory.
   :restrictions: in ]0 - inf[



