import serial
import re
import time
import tkinter as tk
from ctypes import windll
windll.shcore.SetProcessDpiAwareness(1)

win = tk.Tk()
win.title("Arduino GUI")
win.geometry("400x200")     
message = tk.Label(win, text="Hello, World!")
message.pack()
win.mainloop()

# Configurez le port série (remplacez 'COM7' par le port utilisé, ou '/dev/ttyUSB0' sous Linux)
ser = serial.Serial('COM7', 9600, timeout=1)

def read_message(timeout=3):
    """Lit un message complet envoyé par l'Arduino avec un timeout."""
    pattern = r"^<IP=\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}, Method=[A-Za-z0-9]+, Heure=\d{1,2}, Minute=\d{1,2}>$"
    start_time = time.time()
    while True:
        if ser.in_waiting > 0:  # Vérifie si des données sont disponibles
            message = ser.readline().decode('utf-8', errors='ignore').strip()
            if re.match(pattern, message):  # Vérifie si le message correspond au format attendu
                return message
            else:
                print(f"Message ignoré (format incorrect)")
        if time.time() - start_time > timeout:  # Vérifie si le timeout est dépassé
            raise TimeoutError("Aucun message reçu dans le délai imparti.")

while True:
    try:
        userinput = int(input("1: écrire un log | 2: lire un log | 3: effacer tous les logs\nVotre choix : "))
        
        if userinput == 1:
            # Saisie des informations pour écrire un log
            aipinput = int(input("1: Entrez votre IP (premier nombre, par ex 172) : "))
            bipinput = int(input("2: Entrez votre IP (second nombre, par ex 168) : "))
            cipinput = int(input("3: Entrez votre IP (troisième nombre, par ex 1) : "))
            dipinput = int(input("4: Entrez votre IP (quatrième nombre, par ex 1) : "))
            methodinput = input("5: Entrez la méthode : ")
            
            # Envoi de la commande à l'Arduino
            ser.write(f"<{methodinput},{aipinput},{bipinput},{cipinput},{dipinput},1>".encode('utf-8'))
            print("Message transmis !")
        
        elif userinput == 2:
            # Lecture d'un log spécifique
            indexinput = int(input("Entrez l'index du log que vous voulez lire : "))
            ser.write(f"<read,{indexinput}>".encode('utf-8'))
            print("En attente de lecture...")
            
            # Lecture de la réponse de l'Arduino
            try:
                response = read_message()
                print("Réponse Arduino :", response)
            except TimeoutError as e:
                print(e)
        
        elif userinput == 3:
            # Effacement des logs
            ser.write(f"<clear>".encode('utf-8'))
            print("Commande d'effacement envoyée.")
            
            # Lecture de la confirmation de l'Arduino
            try:
                response = read_message()
                print("Réponse Arduino :", response)
            except TimeoutError as e:
                print(e)
        
        else:
            print("Choix invalide. Veuillez entrer 1, 2 ou 3.")
    
    except ValueError:
        print("Erreur : Veuillez entrer un nombre valide.")
    except serial.SerialException as e:
        print(f"Erreur de communication série : {e}")
        break