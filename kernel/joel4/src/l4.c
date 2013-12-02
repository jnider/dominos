/** L4 Generic helper functions */

#include "l4.h"

int L4_ValidateKIPMagic(const L4_KIP* pKip)
{
   if ((pKip->magic[0] != KIP_MAGIC_0) ||
       (pKip->magic[1] != KIP_MAGIC_1) ||
       (pKip->magic[3] != KIP_MAGIC_3))
      return 1;

   return 0;
}

