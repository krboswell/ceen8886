#ifndef __HW2_H__
#define __HW2_H__

extern const unsigned char S_BOX[16][16];
extern const unsigned char S_BOX_INV[16][16];
extern const unsigned int RCON[255];

int hex2int(char *achar, int *aint);
int hex2ascii(unsigned char *key);

#endif
