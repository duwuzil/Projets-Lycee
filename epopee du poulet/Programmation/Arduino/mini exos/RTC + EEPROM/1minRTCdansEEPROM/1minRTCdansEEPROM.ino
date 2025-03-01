#include <93C46.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;
eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);

bool longMode = EEPROM_93C46_MODE_8BIT;
bool parse = false;
bool config = false;

void setup() {
  e.set_mode(longMode);
  Serial.begin(9600);

  writertc();

  Serial.println("resetting data...");
  e.ew_enable();
  e.erase_all();
  //PARTIE 1 ECRITURE
  e.ew_disable();
  /*
  String writeBuffer;
  if (longMode) {
    writeBuffer = "abcde\0";
  } else {
    writeBuffer = "ip:127.0.0.1\0";
  }

  int len = longMode ? 64 : 128;
  // Write your data
  for (int i = 0; i < len; i++) {
    e.write(i, writeBuffer[i]);
  }

  // Optionally, disable EW after writing
 
  //---PARTIE 2 LECTURE---
  Serial.println("Reading data...\n");
  word readBuffer[len];
  for (int i = 0; i < len; i++) {
    word r = e.read(i);
    readBuffer[i] = r;
    Serial.print(char(r));
  }
  debugPrint(readBuffer, len);
  Serial.println();
  */
}

void loop() {



  if (RTC.read(tm)) {  //PARTIE 1 ECRITURE
    e.ew_enable();
    e.erase_all();
    String writeBuffer;
    if (longMode) {
      writeBuffer = "abcde\0";
    } else {
      writeBuffer = "date:" + String(tm.Hour) + "H" + String(tm.Minute) + "m" + String(tm.Second) + "s\0";
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
  delay(60000);
}



void debugPrint(word *buff, int len) {
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

bool getTime(const char *str) {
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str) {
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

void writertc() {
  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    // and configure the RTC with this info
    if (RTC.write(tm)) {
      config = true;
    }
  }

  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
}