String trame[5];
int i=0;

void setup() {
  Serial.begin(9600);     // Démarre la communication série à 9600 bauds
  Serial.setTimeout(100); // Définir un timeout de 100 millisecondes pour la lecture série
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readString();  // Lire la chaîne jusqu'à la fin de la ligne (nouvelle ligne)
      // Afficher la donnée reçue pour debug
    trame[i] = data;
    i = 1 + i % 5;
    Serial.println("method:"+trame[0]+" file:"+trame[1]+" ip add:"+trame[2]+" mac add:"+trame[3]+" Response Code:"+trame[4]);

  }
  
  
  
}
