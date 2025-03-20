#include <TimeLib.h>
#include <DS1307RTC.h>
#include <93C46.h>

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
char method[10] = {};
byte ip[4] = {0, 0, 0, 0};
byte minu = 0;
byte heu = 0;
char newmethod[10] = {};
byte newip[4] = {0, 0, 0, 0};
byte newminu = 0;
byte newheu = 0;
int code = 0; // 1 = écriture, 2 = lecture dernière, 3 = reset, 4 = lecture d'un enregistrement spécifique
boolean newData = false;

int recordToRead = 0;

tmElements_t tm;

#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

bool longMode = EEPROM_93C46_MODE_8BIT;
int add = 0;
int newadd = 0;
int alladd[128] = {0};
int iter = 0;
eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);

void setup() {
  e.set_mode(longMode);
  Serial.begin(9600);
  Serial.println("Programme Arduino prêt");
  delay(200);
}

void loop() {
  if (RTC.read(tm)) {
    minu = tm.Minute;
    heu = tm.Hour;
  }

  recvWithStartEndMarkers();
  if (newData) {
    strcpy(tempChars, receivedChars);
    parseData();
    newData = false;
  }

  if (code == 1) { // Écriture d'un enregistrement
    showParsedData();
    e.ew_enable();

    for (int i = 0; i < 4; i++) {
      e.write(add, ip[i]);
      add += sizeof(ip[i]);
    }

    int methodLen = strlen(method);
    for (int i = 0; i < methodLen; i++) {
      e.write(add, method[i]);
      add += sizeof(method[i]);
    }

    e.write(add, heu);
    add += sizeof(heu);
    e.write(add, minu);
    add += sizeof(minu);

    e.write(add, methodLen);
    alladd[iter] = add;
    iter++;
    add += sizeof(byte);

    e.ew_disable();
    Serial.println("Enregistrement terminé");
    code = 0;
  }

  if (code == 2) { // Lecture de l'enregistrement le plus récent
    if (iter <= 0) {
      Serial.println("<Aucun enregistrement disponible>");
      code = 0;
      return;
    }
    newadd = alladd[iter - 1];
    readRecord();
    code = 0;
  }

  if (code == 4) { // Lecture d'un enregistrement spécifique
    if (recordToRead < 0 || recordToRead >= iter) {
      Serial.println("<Enregistrement non disponible>");
      code = 0;
      return;
    }
    newadd = alladd[recordToRead];
    readRecord();
    code = 0;
  }

  if (code == 3) { // Réinitialisation
    add = 0;
    iter = 0;
    for (int i = 0; i < 128; i++) {
      alladd[i] = 0;
    }
    resetep();
    Serial.println("<Logs effacés>");
    code = 0;
  }
}

void readRecord() {
  int len = e.read(newadd);
  newadd -= sizeof(byte);

  newminu = e.read(newadd);
  newadd -= sizeof(byte);

  newheu = e.read(newadd);
  newadd -= sizeof(byte);

  for (int i = len - 1; i >= 0; i--) {
    newmethod[i] = e.read(newadd);
    newadd -= sizeof(char);
  }
  newmethod[len] = '\0';

  for (int i = 3; i >= 0; i--) {
    newip[i] = e.read(newadd);
    newadd -= sizeof(byte);
  }

  Serial.print("<IP=");
  Serial.print(newip[0]); Serial.print(".");
  Serial.print(newip[1]); Serial.print(".");
  Serial.print(newip[2]); Serial.print(".");
  Serial.print(newip[3]);
  Serial.print(", Method=");
  Serial.print(newmethod);
  Serial.print(", Heure=");
  Serial.print(newheu);
  Serial.print(", Minute=");
  Serial.print(newminu);
  Serial.println(">");
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
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
  char* token = strtok(tempChars, ",");
  if (strcmp(token, "read") == 0) {
    token = strtok(NULL, ",");
    recordToRead = atoi(token);
    code = 4;
  } else if (strcmp(token, "clear") == 0) {
    code = 3;
  } else {
    strcpy(method, token);
    for (int i = 0; i < 4; i++) {
      token = strtok(NULL, ",");
      ip[i] = atoi(token);
    }
    token = strtok(NULL, ",");
    code = atoi(token);
  }
}

void resetep() {
  e.ew_enable();
  e.write_all(0x00);
  e.ew_disable();
}

void showParsedData() {
  Serial.print("Method=");
  Serial.print(method);
  Serial.print(", IP=");
  for (int i = 0; i < 4; i++) {
    Serial.print(ip[i]);
    if (i < 3) Serial.print(".");
  }
  Serial.println("");
}