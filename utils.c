#include "utils.h"

int get_opcode(char * name)
{
  static instruction instructions [] ={{"add",0,1,R},{"sum",45,0,J}};
  int i = 0;
  while(i < sizeof(instructions)/sizeof(instruction))
  {
    if(strcmp(name,instructions[i].name)==0)
    {
      return instructions[i].opcode;
    }
    i++;
  }
  return -1;
}
