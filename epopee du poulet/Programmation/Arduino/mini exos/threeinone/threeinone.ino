#include <93C46.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
char messageFromPC[numChars] = { 0 };
int integerFromPC = 0;
float floatFromPC = 0.0;
int rwop = 0;
boolean newData = false;

tmElements_t tm;
eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);

bool longMode = EEPROM_93C46_MODE_8BIT;
bool parse = false;
bool config = false;


void setup() {
  // put your setup code here, to run once:
  e.set_mode(longMode);
  Serial.begin(9600);
  delay(200);
  e.ew_enable();
  e.erase_all();
  //PARTIE 1 ECRITURE
  e.ew_disable();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (RTC.read(tm)) {
    int heure = print2digits(tm.Hour);
    int minute = print2digits(tm.Minute);
    int seconde = print2digits(tm.Second);
    Serial.println(String(heure) + "H" + String(minute) + "m" + String(seconde) + "s");
    e.ew_enable();
    String writeBuffer;
    if (longMode) {
      writeBuffer = "abcde\0";
    } else {
      writeBuffer = String(tm.Hour) + "|" + String(tm.Minute) + "|" + String(tm.Second) + "|" + messageFromPC + "|" + String(integerFromPC) + "|" + String(floatFromPC);
    }

    int len = longMode ? 64 : 128;
    // Write your data
    for (int i = 0; i < len; i++) {
      e.write(i, writeBuffer[i]);
    }



    // Optionally, disable EW after writing
    e.ew_disable();

    Serial.println("Reading data...\n");
    word readBuffer[len];
    for (int i = 0; i < len; i++) {
      word r = e.read(i);
      readBuffer[i] = r;
      Serial.print(char(r));
    }
    debugPrint(readBuffer, len);
    Serial.println();
  }

  else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);

  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    newData = false;
  }
  if (rwop == 1) {
    Serial.println(String(messageFromPC));
  }

  delay(100);
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;


  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';  // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

int print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  return number;
}

void parseData() {  // split the data into its parts

  char* strtokIndx;  // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");  // get the first part - the string
  strcpy(messageFromPC, strtokIndx);    // copy it to messageFromPC

  strtokIndx = strtok(NULL, ",");    // this continues where the previous call left off
  integerFromPC = atoi(strtokIndx);  // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  floatFromPC = atof(strtokIndx);  // convert this part to a float

  strtokIndx = strtok(NULL, ",");  // this continues where the previous call left off
  rwop = atoi(strtokIndx);
}

//============

void showParsedData() {
  Serial.print("Message ");
  Serial.println(messageFromPC);
  Serial.print("Integer ");
  Serial.println(integerFromPC);
  Serial.print("Float ");
  Serial.println(floatFromPC);
}


void debugPrint(word* buff, int len) {
  Serial.print("\n\t00\t01\t02\t03\t04\t05\t06\t07\t08\t09\t0A\t0B\t0C\t0D\t0E\t0F");
  for (int i = 0; i < len; i++) {
    if (i % 16 == 0) {
      Serial.println();
      Serial.print(i, HEX);
    }
    Serial.print("\t");
    if (buff[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(buff[i], HEX);
  }
}