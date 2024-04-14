#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <time.h>
#include <MY_HANFONT.h>
#include <BluetoothSerial.h>

#include <Fonts/KSFont.h>  
#include <Fonts/ASCFont.h> 

#define TFT_MOSI            23
#define TFT_SCLK            18
#define TFT_CS              15
#define TFT_DC              2
#define TFT_RST             4

String ssid;
String password;

int GMTOffset = 60 * 60 * 9; 
int daylightOffset = 0;
const String weekDays[7] = {"son", "mon", "tue", "wed", "thu", "fri", "sat"};
String buffer;

const String setUpMessage = "기기의 전원이 연결되었습니다. /r와이파이 이름과 비밀번호를 입력해주세요.";
const String setUpException = "와이파이 정보가 올바르지 않습니다. /r전원 케이블을 다시 연결해 주세요."; 

TFT_eSPI tft = TFT_eSPI(240, 320); 
BluetoothSerial SerialBT;

void digitalTimeClock() {
    
    time_t rawtime = time(nullptr);
    struct tm* timeinfo = localtime(&rawtime);
    buffer = weekDays[timeinfo->tm_wday];

    int xpos = 50;
    int ypos = 65; 
    int Ypos = 120;
    int Xpos = 65;
    
    //시(hour)
    if (timeinfo->tm_hour < 10) {
      xpos += tft.drawChar('0', xpos, ypos, 6);
    }
    xpos += tft.drawNumber(timeinfo->tm_hour, xpos, ypos, 6);
    xpos += tft.drawChar(':', xpos, ypos, 6);

    //분(minute)
    if (timeinfo->tm_min < 10) {
      xpos += tft.drawChar('0', xpos, ypos, 6);
    }
    xpos += tft.drawNumber(timeinfo->tm_min, xpos, ypos, 6);
    xpos += tft.drawChar(':', xpos, ypos, 6);

    //초(second)
    if (timeinfo->tm_sec < 10) {
      xpos += tft.drawChar('0', xpos, ypos, 6); 
    }// Add leading zero
    tft.drawNumber(timeinfo->tm_sec, xpos, ypos, 6); 

    //날짜(date)
    Xpos += tft.drawNumber(timeinfo->tm_year + 1900, Xpos, Ypos, 4);
    Xpos += tft.drawChar('/', Xpos, Ypos, 4);
    Xpos += tft.drawNumber(timeinfo->tm_mon + 1, Xpos, Ypos, 4);
    Xpos += tft.drawChar('/', Xpos, Ypos, 4);
    Xpos += tft.drawNumber(timeinfo->tm_mday, Xpos, Ypos, 4);
    Xpos += tft.drawChar(' ', Xpos, Ypos, 4);
    Xpos += tft.drawChar(' ', Xpos, Ypos, 4);
    Xpos += tft.drawChar(' ', Xpos, Ypos, 4);
    Xpos += tft.drawChar(' ', Xpos, Ypos, 4);
    Xpos += tft.drawChar(buffer[0], Xpos, Ypos, 4); 
    Xpos += tft.drawChar(buffer[1], Xpos, Ypos, 4);
    Xpos += tft.drawChar(buffer[2], Xpos, Ypos, 4);

    delay(1000); 

}

void printHandlerMessage(String strr) {
  tft.init(); 
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE); 

  char paramChar[strr.length()+1];
  strr.toCharArray(paramChar, strr.length()+1);
  matrixPrint(0,30,paramChar);
}

String readSerial() {
  String str = "";
  char ch;
  while(SerialBT.available()){
    ch = SerialBT.read();
    str.concat(ch);
    delay(10);
  }
  return str;
}

void setup()
{
    int countException = 0;
    Serial.begin(115200);
    SerialBT.begin("ESP32test");

    Serial.println(F("Start Bluetooth Connection"));
    printHandlerMessage(setUpMessage);

    for(int i = 0; i < 2; i++) {
      if(SerialBT.available()){
        if(i == 0) {
          ssid = readSerial();
          Serial.println(ssid);
        }
        else {
          password = readSerial();
          Serial.println(password);
        }
      }
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println(F("Connecting......"));
      countException++;
    }

    if(countException <= 100) { //와이파이 연결 100초 이내에 연결 시
      Serial.println(F("Connected to Wifi!"));
      configTime(GMTOffset, daylightOffset, "pool.ntp.org", "time.nist.gov");
      delay(1000); 
    }
    else {
      printHandlerMessage(setUpException);
    }
    
     tft.init();  
     tft.setTextSize(1);
     tft.setTextColor(TFT_YELLOW, TFT_BLACK);
     tft.fillScreen(TFT_BLACK);
}

void loop()
{
  digitalTimeClock(); 
}
