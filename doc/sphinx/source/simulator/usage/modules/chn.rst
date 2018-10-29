Channel parameters
------------------

``--blk-fad`` + *TEXT*

   :description: Block fading policy for the RAYLEIGH channel.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `FRAME`
         * `NO`
         * `ONETAP`




``--path`` + *FILE*

   :description: Path to a noisy file, to use with "\\-\\-type USER,OPTICAL" or to a gain file (used with "\\-\\-type RAYLEIGH_USER").



``--implem`` + *TEXT*

   :description: Select the implementation of the algorithm to generate noise.
   :restrictions: 

      .. hlist::
         :columns: 3

         * `FAST`
         * `STD`




``--type`` + *TEXT*

   :description: Type of the channel to use in the simulation ('USER' has an output got from a file when 'USER_ADD' has an additive noise got from a file).
   :restrictions: 

      .. hlist::
         :columns: 3

         * `AWGN`
         * `BEC`
         * `BSC`
         * `NO`
         * `OPTICAL`
         * `RAYLEIGH`
         * `RAYLEIGH_USER`
         * `USER`
         * `USER_ADD`




``--gain-occur`` + *UINT*

   :description: The number of times a gain is used on consecutive symbols (used with "\\-\\-type RAYLEIGH_USER")
   :restrictions: in ]0 - inf[



