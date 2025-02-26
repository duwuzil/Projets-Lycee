#include <93C46.h>

#define pCS 7
#define pSK 9
#define pDI 10
#define pDO 11

// Prints all words of the buffer
void debugPrint(word* buff, int len) {
  Serial.print("\n\t00\t01\t02\t03\t04\t05\t06\t07\t08\t09\t0A\t0B\t0C\t0D\t0E\t0F");
  for(int i = 0; i < len; i++) {
    if(i % 16 == 0) {
      Serial.println();
      Serial.print(i, HEX);
    }
    Serial.print("\t");
    if(buff[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(buff[i], HEX);
  }
}

void setup() {
  String addmac = "3AD57A47773D";
  String addip = "127.0.0.1";
  String mth = "GET";
  String code = "404";
  String file = "/dossier/pageadmindebug.php";
  bool longMode = EEPROM_93C46_MODE_8BIT;
  
  eeprom_93C46 e = eeprom_93C46(pCS, pSK, pDI, pDO);
  e.set_mode(longMode);
  Serial.begin(9600);
 
  Serial.println("Writing data...");
  // First, enable EW (Erase/Write)
  e.ew_enable();
  e.erase_all();
  String writeBuffer;
  if(longMode) {
    writeBuffer = "abcde\0";
  } else {
    writeBuffer = "mac:" + addmac + "|ip:"+ addip + "|mth:"+mth+"|code:"+code+"|file:"+file+"\0";

  }

  int len = longMode ? 64 : 128;
  // Write your data
  for(int i = 0; i < len; i++) {
    e.write(i, writeBuffer[i]);
  }

  // Optionally, disable EW after writing
  e.ew_disable();

  Serial.println("Reading data...\n");
  word readBuffer[len];
  for(int i = 0; i < len; i++) {
    word r = e.read(i);
    readBuffer[i] = r;
    Serial.print(char(r));
  }
  debugPrint(readBuffer, len);
  Serial.println();
}

void loop() {}
