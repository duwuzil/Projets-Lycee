import serial
import time

# Configure la communication série avec l'Arduino (changez 'COM3' selon votre configuration)
ser = serial.Serial('/dev/ttyACM0', 9600)  # Ou '/dev/ttyUSB0' sur Linux/Mac
time.sleep(2)  # Attendre que la connexion soit établie

# Envoie d'un message
ser.write("<want drugs, 10,0.2,0>".encode("utf-8"))  # Envoie d'une chaîne à l'Arduino
time.sleep(1)  # Attendre un peu pour s'assurer que l'Arduino ait le temps de répondre

# Lire la réponse de l'Arduino
while ser.in_waiting > 0:
    response = ser.readline().decode('utf-8').strip()
    print("Réponse de l'Arduino:", response)

ser.close()  # Ferme la communication série
