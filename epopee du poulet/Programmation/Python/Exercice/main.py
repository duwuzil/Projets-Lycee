#!/usr/bin/env python3
import serial
import time

def send_data(comport, baudrate):
    ser = serial.Serial(comport, baudrate, timeout=0.1)  # Ouvre le port série
    ip = "192.168.1.9"  # Exemple d'IP
    mac = "00:1A:2B:3C:4D:5E"  # Exemple de MAC
    code = "302"  # Exemple de code
    method = "POST"
    file = "/dossier/fichier.php?=login:admin&password:admin"

    # Crée une chaîne avec IP, MAC et code séparés par des sauts de ligne
    myip = f"{ip}"
    mymac = f"{mac}"
    mycode = f"{code}"
    mymethod = f"{method}"
    myfile = f"{file}"

    print(f"Envoi des données : {myip} {mymac} {mycode} {mymethod} {myfile}")  # Affiche les données envoyées pour vérification

    # Envoi des données à l'Arduino
    ser.write(myip.encode('utf-8'))
    time.sleep(0.2)
    ser.write(mymac.encode('utf-8'))
    time.sleep(0.2)
    ser.write(mycode.encode('utf-8'))
    time.sleep(0.2)
    ser.write(mymethod.encode('utf-8'))
    time.sleep(0.2)
    ser.write(myfile.encode('utf-8'))
    time.sleep(0.2)
    # Attends que l'Arduino réponde
    time.sleep(1)

    # Lecture des données reçues de l'Arduino (si nécessaire)
    data_received = ser.readline().decode().strip()
    if data_received:
        print(f"Données reçues : {data_received}")



    ser.close()

if __name__ == '__main__':
    send_data('/dev/ttyACM1', 9600)
