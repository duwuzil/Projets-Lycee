#include <TimeLib.h>
#include <DS1307RTC.h>

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
char messageFromPC[numChars] = {  };
boolean newData = false;

tmElements_t tm;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("bijour");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (RTC.read(tm)) {
    int heure = print2digits(tm.Hour);
    int minute = print2digits(tm.Minute);
    int seconde = print2digits(tm.Second);
    Serial.println(String(heure) + "H" + String(minute) + "m" + String(seconde) + "s");
  }
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    newData = false;
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


void parseData() {  // split the data into its parts

  char* strtokIndx;  // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");  // get the first part - the string
  strcpy(messageFromPC, strtokIndx);    // copy it to messageFromPC

}


int print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  return number;
}

//============

void showParsedData() {
  Serial.print(messageFromPC);
}
