#include "mySimpleComputer.h"

int sc_regInit(void){
    FLAG = 0;
    return 0;
}
int sc_regSet(int Register, int value){
        if (Register != FLAG_OVERFLOW && Register != FLAG_DIVISION && 
            Register != FLAG_OUTOFMEM && Register != FLAG_INVALID && 
            Register != FLAG_STOP) {
                value = -1;
            return value; 
        }
    
        if (value) {
            FLAG |= Register;
        } else {
            FLAG &= ~Register;
        }
        value = 0;
        return value; 
}

int sc_regGet(int Register, int *value) {
    if (!value)
    {
      return -1;
    }

    if (Register == FLAG_OVERFLOW || Register == FLAG_DIVISION
      || Register == FLAG_OUTOFMEM || Register == FLAG_INVALID
      || Register == FLAG_STOP)
    {
      *value = (FLAG & Register) ? 1 : 0;
      return 0;
    }
    else if (Register == 0)
    {
      *value = FLAG;
      return 0;
    }

  return -1;
}
int sc_accumulatorInit(void)
{
    acum = 0;
    return 0;
}
int sc_accumulatorSet(int value)
{
    if ((0 > value) || (value > 10000)) { return -1; }
    else { acum = value;  return 0; }
}
int sc_accumulatorGet(int* value)
{
    if (&acum == value){return *value;}
    else{return -1;}
}
int sc_icounterInit(void)
{
    schet = 0;
    return 0;
}
int sc_icounterSet(int value)
{
    if(schet < 0) { return -1;}
    else { schet = value; return 0; }
}
int sc_icounterGet(int* value)
{
    if (&schet == value){ return *value;}
    else {return -1;}
}