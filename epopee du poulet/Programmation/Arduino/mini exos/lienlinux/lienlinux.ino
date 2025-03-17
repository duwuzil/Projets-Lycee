#include <TimeLib.h>
#include <DS1307RTC.h>
#include <93C46.h>

#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
char messageFromPC[numChars] = {};
int code = 0;
bool newData = false;

int add = 0;

bool longMode = EEPROM_93C46_MODE_8BIT;
eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);

tmElements_t tm;

String heure = "";
String minute = "";

void setup() {
  Serial.begin(9600);
  Serial.println("Programme final en lancement");
  e.set_mode(longMode);
}

void loop() {
  if (RTC.read(tm)) {
    heure = String(tm.Hour);
    minute = String(tm.Minute);
  }
  recvWithStartEndMarkers();
  if (newData) {
    strcpy(tempChars, receivedChars);
    parseData();
    newData = false;
  }

  if (code == 1) {
    showParsedData();
    writeToEEPROM();
    delay(100);
    code = 0;
  }
}

void recvWithStartEndMarkers() {
  static bool recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && !newData) {
    rc = Serial.read();

    if (recvInProgress) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void parseData() {
  char* strtokIndx;
  strtokIndx = strtok(tempChars, ",");
  strcpy(messageFromPC, strtokIndx);

  strtokIndx = strtok(NULL, ",");
  if (strtokIndx != NULL) {
    code = atoi(strtokIndx);
  }
}

void writeToEEPROM() {
  e.ew_enable();
  String writeBuffer;
  
  if (longMode) {
    writeBuffer = "This is a string written in the 16-bit organization.\nHi, world!\0";
  } else {
    writeBuffer = heure + minute;
  }

  int len = longMode ? 64 : writeBuffer.length();
  for (int i = 0; i < len; i++) {
    e.write(i, writeBuffer[i]);
  }

  e.ew_disable();
}

void showParsedData() {
  Serial.println(messageFromPC);
}
