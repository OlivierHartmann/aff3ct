Source parameters
-----------------

``--type`` + TEXT

   :description: Method used to generate the codewords.
   :restrictions: in {AZCW,RAND,USER}



``-F,--fra`` + UINT

   :description: Set the number of inter frame level to process.
   :restrictions: in ]0 - inf[



``--start-idx`` + UINT

   :description: 0          Start idx to use in the USER type source.



``--path`` + FILE

   :description: Path to a file containing one or a set of pre-computed source bits (to use with "\\-\\-type USER").



``--implem`` + TEXT

   :description: Select the implementation of the algorithm to generate the information bits.
   :restrictions: in {FAST,STD}



