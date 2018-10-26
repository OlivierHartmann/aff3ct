Modem parameters
----------------

``--cpm-ws`` + TEXT

   :description: Wave shape.
   :restrictions: in {GMSK,RCOS,REC}



``--const-path`` + FILE

   :description: Path to the ordered modulation symbols (constellation), to use with "\\-\\-type USER".



``--cpm-k`` + UINT

   :description: Modulation index numerator.
   :restrictions: in ]0 - inf[



``--cpm-std`` + TEXT

   :description: The selection of a default CPM standard hardly implemented (those parameters overwrite the other arguments given by the user).
   :restrictions: in {GSM}



``--cpm-p`` + UINT

   :description: Modulation index denominator.
   :restrictions: in ]0 - inf[



``--cpm-map`` + TEXT

   :description: Symbols mapping layout.
   :restrictions: in {GRAY,NATURAL}



``--bps`` + UINT

   :description: Select the number of bits per symbol.
   :restrictions: in ]0 - inf[



``--ite`` + UINT

   :description: Number of iteration in the demodulator.
   :restrictions: in ]0 - inf[



``--psi`` + TEXT

   :description: Select the type of the psi function to use in the SCMA demodulator.
   :restrictions: in {PSI0,PSI1,PSI2,PSI3}



``--max`` + TEXT

   :description: Select the type of the max operation to use in the demodulator.
   :restrictions: in {MAX,MAXL,MAXS,MAXSS}



``--ups`` + UINT

   :description: Select the symbol sampling factor.
   :restrictions: in ]0 - inf[



``--no-sig2``

   :description: Turn off the division by sigma square in the demodulator.



``--type`` + TEXT

   :description: Type of the modulation to use in the simulation.
   :restrictions: in {BPSK,CPM,OOK,PAM,PSK,QAM,SCMA,USER}



``--implem`` + TEXT

   :description: Select the implementation of the modem.
   :restrictions: in {FAST,STD}



``--cpm-L`` + UINT

   :description: CPM pulse width or CPM memory.
   :restrictions: in ]0 - inf[



