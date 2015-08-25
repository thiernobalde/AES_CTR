#ifndef _EAS_
#define _EAS_
#include "util.h"

void KeyExpansion (unsigned char ckey[4][4],unsigned char keySchedule[4][44]);
void eas_encryption(unsigned char plaintext[16], unsigned char keySchedule[4][44],unsigned char cypherText[16]);
void eas_decryption(unsigned char cypherText[16], unsigned char keySchedule[4][44], unsigned char plainText[16]);

#endif

