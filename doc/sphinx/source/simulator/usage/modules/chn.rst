Channel parameters
------------------

``--type`` + TEXT

   :description: Type of the channel to use in the simulation ('USER' has an output got from a file when 'USER_ADD' has an additive noise got from a file).
   :restrictions: in {AWGN,BEC,BSC,NO,OPTICAL,RAYLEIGH,RAYLEIGH_USER,USER,USER_ADD}



``--blk-fad`` + TEXT

   :description: Block fading policy for the RAYLEIGH channel.
   :restrictions: in {FRAME,NO,ONETAP}



``--path`` + FILE

   :description: Path to a noisy file, to use with "\\-\\-type USER,OPTICAL" or to a gain file (used with "\\-\\-type RAYLEIGH_USER").



``--gain-occur`` + UINT

   :description: The number of times a gain is used on consecutive symbols (used with "\\-\\-type RAYLEIGH_USER")
   :restrictions: in ]0 - inf[



``--implem`` + TEXT

   :description: Select the implementation of the algorithm to generate noise.
   :restrictions: in {FAST,STD}



