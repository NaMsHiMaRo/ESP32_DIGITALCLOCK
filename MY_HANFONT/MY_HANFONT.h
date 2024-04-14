#ifndef MY_HANFONT_H
#define MY_HANFONT_H

#include <Arduino.h>

byte *getHAN_font(byte HAN1, byte HAN2, byte HAN3);

void matrixPrint(int XPOS,int YPOS,char *pChar);


#endif
