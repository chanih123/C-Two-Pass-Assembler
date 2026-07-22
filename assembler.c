#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
int IC,DC; 

int main()
{
   int x = get_opcode("add");
   int y = get_opcode("sub");
   printf("%d\n",x);
   printf("%d\n",y);
   return 0;
}
