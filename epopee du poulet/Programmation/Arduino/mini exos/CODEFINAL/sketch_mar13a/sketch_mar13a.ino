#include <TimeLib.h>
#include <DS1307RTC.h>
#include <93C46.h>

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
char method[10] = {};
byte ip[4] = { 0, 0, 0, 0 };
byte minu = 0;
byte heu = 0;
char newmethod[10] = {};
char invertmethod[10] = {};
byte newip[4] = { 0, 0, 0, 0 };
byte newminu = 0;
byte newheu = 0;
int code = 0;
boolean newData = false;

tmElements_t tm;

#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

bool longMode = EEPROM_93C46_MODE_8BIT;
int add = 0;
int newadd = 0;
eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);


void setup() {
  e.set_mode(longMode);
  Serial.begin(9600);

  Serial.println("programme final en lancement");
  delay(200);
}

void loop() {
  if (RTC.read(tm)) {
    minu = tm.Minute;
    heu = tm.Hour;
  }
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    parseData();
    newData = false;
  }

  if (code == 1) {
    showParsedData();
    e.ew_enable();


    e.write(add, ip[0]);
    add += sizeof(ip[0]);
    e.write(add, ip[1]);
    add += sizeof(ip[1]);
    e.write(add, ip[2]);
    add += sizeof(ip[2]);
    e.write(add, ip[3]);
    add += sizeof(ip[3]);
    for (int i = 0; i < String(method).length(); i++) {
      e.write(add + i, method[i]);
    }
    add += String(method).length();

    e.write(add, heu);
    add += sizeof(heu);

    e.write(add, minu);
    add += sizeof(minu);

    e.write(add, "\0");

    Serial.println(add);
    newadd = add;

    // Optionally, disable EW after writing
    e.ew_disable();

    delay(100);
    code = 0;
  }
  if (code == 2) {
    newadd -= 1;

    newminu = e.read(newadd);
    newadd -= sizeof(newminu);

    newheu = e.read(newadd);
    newadd -= sizeof(newheu);


    for (int i = String(method).length() - 1; i >= 0; i--) {
      newmethod[i] = e.read(newadd);   // Lire chaque caractère
      newadd -= sizeof(newmethod[i]);  // Décrémenter l'adresse après chaque lecture
    }




    newip[3] = e.read(newadd);
    newadd -= sizeof(newip[3]);
    newip[2] = e.read(newadd);
    newadd -= sizeof(newip[2]);
    newip[1] = e.read(newadd);
    newadd -= sizeof(newip[1]);
    newip[0] = e.read(newadd);
    newadd -= sizeof(newip[0]);

    Serial.println(newip[0]);
    Serial.println(newip[1]);
    Serial.println(newip[2]);
    Serial.println(newip[3]);
    Serial.println(newmethod);
    Serial.println(newheu);
    Serial.println(newminu);
    code = 0;
  }
  if (code == 3) {
    add = 0;
    resetep();
    delay(100);
    code = 0;
  }
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
        receivedChars[ndx] = '\0';
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

void parseData() {
  char* strtokIndx;
  strtokIndx = strtok(tempChars, ",");
  strcpy(method, strtokIndx);

  strtokIndx = strtok(NULL, ",");
  for (int i = 0; i < 4; i++) {
    ip[i] = atoi(strtokIndx);
    strtokIndx = strtok(NULL, ",");
  }

  code = atoi(strtokIndx);
}

void resetep() {
  e.ew_enable();
  e.write_all(0x00);
  e.ew_disable();
}

void showParsedData() {
  Serial.print(method);
  for (int i = 0; i < 4; i++) {
    Serial.println(ip[i]);
  }
}
