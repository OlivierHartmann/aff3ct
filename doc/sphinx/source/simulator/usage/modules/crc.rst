CRC parameters
--------------

``--type,--poly`` + TEXT

   :description: NO       Select the CRC type/polynomial you want to use (ex: "8-DVB-S2": 0xD5, "16-IBM": 0x8005, "24-LTEA": 0x864CFB, "32-GZIP": 0x04C11DB7).



``--size`` + UINT

   :description: Size of the CRC (divisor size in bit -1), required if you selected an unknown CRC.
   :restrictions: in ]0 - inf[



``--implem`` + TEXT

   :description: Select the CRC implementation you want to use.
   :restrictions: in {FAST,INTER,STD}



