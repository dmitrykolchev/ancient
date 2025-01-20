#include "pm.h"

void PMMain()
{
   int i;

   asm sti
   asm pushf
   asm pop ax
   asm or  ax,0x100
   asm push ax
   asm popf

   pmprintf("CS=%Xh", 0x1234);
   asm pushf
   asm pop ax
   asm and ah,0xFE
   asm push ax
   asm popf 
}

void RMMain()
{
}