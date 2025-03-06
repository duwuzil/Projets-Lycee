#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <93C46.h>

#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

bool longMode = EEPROM_93C46_MODE_8BIT;
eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);
int len = longMode ? 64 : 128;

const byte numChars = 64;
char receivedChars[numChars];
char tempChars[numChars];  // temporary array for use when parsing

char Charstemp[numChars];  // temporary array for use when parsing

// variables to hold the parsed data
char file[numChars] = {};
byte ip[4] = { 0, 0, 0, 0 };
byte mac[6] = { 00, 00, 00, 00, 00, 00 };
int code = 0;
char method[numChars] = {};
int on = 0;

tmElements_t tm;

boolean newData = false;

void setup() {
  e.set_mode(longMode);
  Serial.begin(9600);

  Serial.println("Writing data...");
  while (!Serial)
    ;  // wait for serial
  delay(200);
  Serial.println("DS1307RTC Read Test, Serial Comm, 93C46 eeprom");
  Serial.println("-------------------");
  Serial.println("This demo expects 3 pieces of data - text, an integer and a floating point value");
  Serial.println("Enter data in this style <HelloWorld, 12, 24.7>  ");
  Serial.println();
}

void loop() {
  if (RTC.read(tm)) {
    // First, enable EW (Erase/Write)
    recvWithStartEndMarkers();
    if (newData == true) {
      strcpy(tempChars, receivedChars);
      parseData();
      showParsedData();
      
      // Écriture des données dans l'EEPROM après les avoir reçues
      writeDataToEEPROM();
      
      newData = false;
    }

  } else {
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
  delay(100);
  writeDataToEEPROM();

  if(on == 1) {
    readDataFromEEPROM();
    delay(100);
  }
}

//===========

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
    } else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

//===========

void parseData() {  // split the data into its parts
  char* strtokIndx;  // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");  // get the first part - the string
  strcpy(file, strtokIndx);             // copy it to messageFromPC

  for (int i = 0; i < 4; i++) {
    strtokIndx = strtok(NULL, ",");
    ip[i] = atoi(strtokIndx);
  }

  // Extraction de la première partie (premier octet de l'adresse MAC)
  for (int i = 0; i < 6; i++) {
    strtokIndx = strtok(NULL, ",");
    mac[i] = strtol(strtokIndx, NULL, 16);  // Conversion d'une chaîne hexadécimale en entier
  }

  strtokIndx = strtok(NULL, ",");  // get the first part - the string
  strcpy(method, strtokIndx);      // copy it to messageFromPC

  strtokIndx = strtok(NULL, ",");
  code = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  on = atoi(strtokIndx);
}

//===========

void showParsedData() {
  Serial.print("Message ");
  Serial.println(file);
  Serial.print("IP :");
  for (int i = 0; i < 4; i++) {
    Serial.print(ip[i]);
    Serial.print(".");
  }

  Serial.print("\nMAC :");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    Serial.print(":");
  }

  Serial.print("\nMethod :");
  Serial.println(method);

  Serial.print("Code :");
  Serial.println(code);

  Serial.println(String(tm.Hour) + "H" + String(tm.Minute) + "m" + String(tm.Second) + "s");
  Serial.println(String(tm.Day) + "j" + String(tm.Month) + "M" + String(tmYearToCalendar(tm.Year)) + "A");

  Serial.println("\n<jairecu:,12,0.5>");
}

//===========

// Fonction pour écrire les données dans l'EEPROM
void writeDataToEEPROM() {
  int address = 0;

  // Écrire 'file' dans l'EEPROM
  for (int i = 0; i < numChars; i++) {
    e.write(address++, file[i]);
  }

  // Écrire l'IP dans l'EEPROM
  for (int i = 0; i < 4; i++) {
    e.write(address++, ip[i]);
  }

  // Écrire la MAC dans l'EEPROM
  for (int i = 0; i < 6; i++) {
    e.write(address++, mac[i]);
  }

  // Écrire le code dans l'EEPROM (2 octets pour un int)
  e.write(address++, (code >> 8) & 0xFF);   // byte haut
  e.write(address++, code & 0xFF);           // byte bas

  // Écrire la méthode dans l'EEPROM
  for (int i = 0; i < numChars; i++) {
    e.write(address++, method[i]);
  }
}

//===========

// Fonction pour lire les données depuis l'EEPROM
void readDataFromEEPROM() {
  int address = 0;

  // Lire 'file' depuis l'EEPROM
  for (int i = 0; i < numChars; i++) {
    file[i] = e.read(address++);
  }

  // Lire l'IP depuis l'EEPROM
  for (int i = 0; i < 4; i++) {
    ip[i] = e.read(address++);
  }

  // Lire la MAC depuis l'EEPROM
  for (int i = 0; i < 6; i++) {
    mac[i] = e.read(address++);
  }

  // Lire le code depuis l'EEPROM (2 octets pour un int)
  byte highByte = e.read(address++);
  byte lowByte = e.read(address++);
  code = (highByte << 8) | lowByte;

  // Lire la méthode depuis l'EEPROM
  for (int i = 0; i < numChars; i++) {
    method[i] = e.read(address++);
  }
}
