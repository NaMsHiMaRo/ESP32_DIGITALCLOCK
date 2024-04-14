#include "MY_HANFONT.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>

#include <Fonts/KSFont.h>   //한글폰트
#include <Fonts/ASCFont.h>

TFT_eSPI tft1 = TFT_eSPI(240, 320);

byte HANFontImage[32] = {0, }; //한글 출력을 위한 바이트 객체

byte *getHAN_font(byte HAN1, byte HAN2, byte HAN3){


  const byte cho[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 3, 1, 2, 4, 4, 4, 2, 1, 3, 0 };
  const byte cho2[] = { 0, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7, 7, 7, 6, 6, 7, 7, 7, 6, 6, 7, 5 };
  const byte jong[] = { 0, 0, 2, 0, 2, 1, 2, 1, 2, 3, 0, 2, 1, 3, 3, 1, 2, 1, 3, 3, 1, 1 };

  uint16_t utf16;
  byte first, mid, last;
  byte firstType, midType, lastType;
  byte i;
  byte *pB, *pF;

  /*------------------------------
    UTF-8 을 UTF-16으로 변환
  */------------------------------
  utf16 = (HAN1 & 0x0f) << 12 | (HAN2 & 0x3f) << 6 | HAN3 & 0x3f;

  /*------------------------------
    초,중,종성 코드를 분리
  ------------------------------*/
  utf16 -= 0xac00;
  last = utf16 % 28;
  utf16 /= 28;
  mid = utf16 % 21;
  first = utf16 / 21;

  first++;
  mid++;

  /*------------------------------
    초,중,종성 해당 폰트 타입(벌)을 결정
  ------------------------------*/

  if(!last){  //받침 없는 경우
    firstType = cho[mid];
    if(first == 1 || first == 24) midType = 0;
    else midType = 1;
  }
  else{       //받침 있는 경우
    firstType = cho2[mid];
    if(first == 1 || first == 24) midType = 2;
    else midType = 3;
    lastType = jong[mid];
  }
  memset(HANFontImage, 0, 32);

  //초성 
  pB = HANFontImage;
  pF = (byte*)KSFont + (firstType*20 + first)*32;

  i = 32;
  while(i--) *pB++ = pgm_read_byte(pF++); 

  //중성
  pB = HANFontImage;
  pF = (byte*)KSFont + (8*20 + midType*22 + mid)*32;

  i = 32;
  while(i--) *pB++ |= pgm_read_byte(pF++); 

  //종성
  if(last){
    pB = HANFontImage;
    pF = (byte*)KSFont + (8*20 + 4*22 + lastType*28 + last)*32;

    i = 32;
    while(i--) *pB++ |= pgm_read_byte(pF++); 
  }

  return HANFontImage;
}

void matrixPrint(int XPOS,int YPOS,char *pChar){
  byte rg = 3;   //<b1> red, <b0> green
  byte *pFs;
  byte i, b;
  byte c, c2, c3;

  while(*pChar){ 
    c = *(byte*)pChar++;

    // /r 개행 문자인지 확인
    if(c == 0x2F && *(byte*)pChar++ == 0x72){
      XPOS = 0;
      YPOS = YPOS+16;
    }
    //---------- 한글 ---------
    else if(c >= 0x80){
      c2 = *(byte*)pChar++;
      c3 = *(byte*)pChar++;
      
      pFs = getHAN_font(c, c2, c3);
      tft1.drawBitmap(XPOS, YPOS,  pFs, 16, 16, 1);
      XPOS = XPOS+16;
      if(XPOS > 240){
        XPOS = 0;
        YPOS = YPOS+16;
      }
    }
    //---------- ASCII ---------
    else{
      pFs = (byte*)ASCfontSet + ((byte)c - 0x20) * 16;
      tft1.drawBitmap(XPOS, YPOS,  pFs, 8, 16, 1);
      XPOS = XPOS+8;
      if(XPOS > 240){
        XPOS = 0;
        YPOS = YPOS+16;
      }   
    }   
  }  
}  

