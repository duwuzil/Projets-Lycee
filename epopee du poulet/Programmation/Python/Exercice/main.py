import serial
import time

# Variables pour contenir les données analysées
receivedChars = ""
tempChars = ""  # Utilisé temporairement pour l'analyse

messageFromPC = ""
integerFromPC = 0
floatFromPC = 0.0

newData = False

# Configuration de la communication série
ser = serial.Serial('/dev/ttyACM1', 9600)  # Changez selon votre configuration
time.sleep(2)  # Attendez un peu pour que la connexion série soit établie
print("Connexion série établie")

# Fonction pour recevoir les données avec des marqueurs de début et de fin
def recvWithStartEndMarkers():
    global receivedChars, newData

    startMarker = '<'
    endMarker = '>'

    recvInProgress = False
    ndx = 0

    while ser.in_waiting > 0 and not newData:
        rc = ser.read().decode('utf-8')  # Lire un octet et le convertir en chaîne
        # print(f"Caractère reçu : {rc}")  # Affiche chaque caractère reçu (pour le débogage)

        if recvInProgress:
            if rc != endMarker:
                receivedChars += rc  # Ajouter le caractère reçu à receivedChars
                ndx += 1
                if ndx >= 64:  # Limiter à la taille max
                    ndx = 63
            else:
                newData = True  # Marqueur de fin atteint
                recvInProgress = False
                # Nettoyer la chaîne après réception complète
                receivedChars = receivedChars.strip('<>')  # Retirer les marqueurs de début et de fin

        elif rc == startMarker:
            recvInProgress = True  # Commencer la réception des données

# Fonction pour analyser les données
def parseData():
    global messageFromPC, integerFromPC, floatFromPC

    # Diviser la chaîne en parties séparées par une virgule
    parts = receivedChars.split(',')

    print(f"Parties extraites : {parts}")  # Affiche les parties extraites pour le débogage

    # Assurez-vous que la chaîne contient bien trois parties avant de procéder à l'analyse
    if len(parts) == 3:
        messageFromPC = parts[0].strip()  # Extraire et nettoyer la première partie (chaîne)
        try:
            integerFromPC = int(parts[1].strip())  # Convertir la deuxième partie en entier
        except ValueError:
            integerFromPC = 0  # Valeur par défaut en cas d'erreur de conversion
        try:
            floatFromPC = float(parts[2].strip())  # Convertir la troisième partie en flottant
        except ValueError:
            floatFromPC = 0.0  # Valeur par défaut en cas d'erreur de conversion

# Fonction pour afficher les données analysées
def showParsedData():
    print(f"Message: {messageFromPC}")
    print(f"Integer: {integerFromPC}")
    print(f"Float: {floatFromPC}")

# Boucle principale
while True:
    ser.write(b"<uwu,12,9>")

    recvWithStartEndMarkers()  # Attendre et recevoir les données
    if newData:
        tempChars = receivedChars  # Copier les données reçues
        print(f"Réception complète: {receivedChars}")  # Affiche les données reçues
        parseData()  # Analyser les données
        showParsedData()  # Afficher les résultats
        newData = False  # Réinitialiser le drapeau newData
        receivedChars = ""  # Réinitialiser pour la prochaine réception
    time.sleep(0.1)  # Attendre un peu avant de recommencer
