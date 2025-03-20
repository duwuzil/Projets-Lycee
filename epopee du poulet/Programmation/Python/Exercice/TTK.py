from tkinter import *
import serial
import re
import time

# Variable globale pour suivre l'état des fenêtres
saveopton = False
loadopton = False
deleteopton = False
global adress
adress = 0


def setport():
    global ser
    ser = serial.Serial(comport.get(), 9600, timeout=1)

# Création de la fenêtre principale
win = Tk()
win.geometry("700x700")
win.title('Data Logs')
win["bg"] = "#1c1726"
win.resizable(height=False, width=False)

# Initialisation de delret après la création de la fenêtre principale
delret = StringVar()  # Initialisation de delret en tant que StringVar()

def read_message(timeout=3):
    """Lit un message complet envoyé par l'Arduino avec un timeout, et extrait uniquement les messages entre < et >."""
    pattern = r"^<IP=\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}, Method=[A-Za-z0-9]+, Heure=\d{1,2}, Minute=\d{1,2}>$|^<.*>$"
    
    start_time = time.time()

    while True:
        if ser.in_waiting > 0:  # Vérifie si des données sont disponibles
            message = ser.readline().decode('utf-8', errors='ignore').strip()
            
            # Extrait le message entre < et > (si présent)
            match = re.search(r'<(.*)>', message)
            if match:
                extracted_message = match.group(0)  # Message complet entre < et >
                if re.match(pattern, extracted_message):  # Vérifie si le message correspond au format attendu
                    delret.set(extracted_message)  # Met à jour la variable globale via StringVar
                    return extracted_message
                else:
                    delret.set("message ignoré.")
                    print(f"Message ignoré - si option 2, log peut être indisponible: {extracted_message}")
            else:
                delret.set("message ignoré.")
                print("message ignoré.")
        
        if time.time() - start_time > timeout:  # Vérifie si le timeout est dépassé
            raise TimeoutError("Aucun message reçu dans le délai imparti.")

def eraser():
    """Envoie la commande d'effacement à l'Arduino et attend la réponse."""
    adress = 0
    ser.write(f"<clear>".encode('utf-8'))
    print("Commande d'effacement envoyée.")
            
    # Lecture de la confirmation de l'Arduino
    try:
        response = read_message()
        print("Réponse Arduino :", response)
        delret.set(response)  # Mise à jour du texte dans la fenêtre avec la réponse reçue
    except TimeoutError as e:
        print(e)

def loader():
    
    ser.write(f"<read,{loadvar.get()}>".encode('utf-8'))
    print("En attente de lecture...")
            
    # Lecture de la réponse de l'Arduino
    try:
        response = read_message()
        print("Réponse Arduino :", response)
        delret.set(response)  # Mise à jour du texte dans la fenêtre avec la réponse reçue
    except TimeoutError as e:
        print(e)


def saver():
    print(methodinput,aipinput,bipinput,cipinput,dipinput)
    global adress
    ser.write(f"<{methodinput.get()},{aipinput.get()},{bipinput.get()},{cipinput.get()},{dipinput.get()},1>".encode('utf-8'))
    print("Envoi en cours | adresse : "+str(adress))
    adress += 1


def SaveOption():
    global saveopton
    if not saveopton:  # Vérifie si une fenêtre de sauvegarde n'est pas déjà ouverte
        saveopton = True
        saveopt = Toplevel(win)  # Crée une nouvelle fenêtre secondaire
        saveopt.geometry("700x700")
        saveopt.title('Save Windows')
        saveopt["bg"] = "#071a47"
        saveopt.resizable(height=False, width=False)
        delret.set("En attente de la réponse de l'Arduino...")
        TReponse = Label(saveopt, text="Réponse de l'arduino :", font=("Verdana", 19, "bold italic"), fg="white", bg='#071a47')
        TReponse.place(x=175,y=50)

        Reponse = Label(saveopt, textvariable=delret, font=("Verdana", 15, ""), fg="white", bg='#071a47')
        Reponse.place(x=175,y=125)

        tot = Label(saveopt, text="entrez la méthode puis \n ensuite l'ip (ex: 172 16 100 1) :", font=("Verdana", 15, ""), fg="white", bg='black')
        tot.place(x=175,y=175)
        global methodinput
        methodinput=StringVar()
        global aipinput
        aipinput=StringVar()
        global bipinput
        bipinput=StringVar()
        global cipinput
        cipinput=StringVar()
        global dipinput
        dipinput=StringVar()
        exin = Label(saveopt, text="get post etc..", font=("Verdana", 15, ""), fg="black", bg='white')
        exin.place(x=250,y=250)
        method = Entry(saveopt,textvariable=methodinput,width=7)
        method.place(x=175,y=250)
        iaexin = Label(saveopt, text="172", font=("Verdana", 12, ""), fg="black", bg='white')
        iaexin.place(x=175,y=300)
        ipa = Entry(saveopt,textvariable=aipinput,width=5)
        ipa.place(x=175,y=325)
        ibexin = Label(saveopt, text="16", font=("Verdana", 12, ""), fg="black", bg='white')
        ibexin.place(x=225,y=300)
        ipb = Entry(saveopt,textvariable=bipinput,width=5)
        ipb.place(x=225,y=325)
        icexin = Label(saveopt, text="100", font=("Verdana", 12, ""), fg="black", bg='white')
        icexin.place(x=275,y=300)
        ipc = Entry(saveopt,textvariable=cipinput,width=5)
        ipc.place(x=275,y=325)
        idexin = Label(saveopt, text="9", font=("Verdana", 12, ""), fg="black", bg='white')
        idexin.place(x=325,y=300)
        ipd = Entry(saveopt,textvariable=dipinput,width=5)
        ipd.place(x=325,y=325)
        dabuton = Button(saveopt, text="load from data", font=("Verdana", 19, "bold italic"), bd=0, command=saver, width=18, height=1, bg="#000000", fg="white", activebackground="#b8e6d2")
        dabuton.place(x=175,y=400)

        # Quand la fenêtre est fermée, réinitialiser saveopton
        saveopt.protocol("WM_DELETE_WINDOW", lambda: close_save_window(saveopt))

def close_save_window(window):
    global saveopton
    saveopton = False
    window.destroy()  # Ferme la fenêtre de sauvegarde

def LoadOption():
    global loadopton
    if not loadopton:  # Vérifie si une fenêtre de chargement n'est pas déjà ouverte
        loadopton = True
        loadopt = Toplevel(win)  # Crée une nouvelle fenêtre secondaire pour le chargement
        loadopt.geometry("700x700")
        loadopt.title('Load Windows')
        loadopt["bg"] = "#214737"
        loadopt.resizable(height=False, width=False)

        delret.set("En attente de la réponse de l'Arduino...")
        TReponse = Label(loadopt, text="Réponse de l'arduino :", font=("Verdana", 19, "bold italic"), fg="white", bg='#214737')
        TReponse.place(x=175,y=50)

        Reponse = Label(loadopt, textvariable=delret, font=("Verdana", 15, ""), fg="white", bg='#214737')
        Reponse.place(x=10,y=125)

        tot = Label(loadopt, text="entrez ici l'index de mémoire :", font=("Verdana", 15, ""), fg="white", bg='black')
        tot.place(x=175,y=350)
        global loadvar
        loadvar = StringVar()
        index = Entry(loadopt,textvariable=loadvar,width=2)
        index.place(x=485,y=352)
        
        dabuton = Button(loadopt, text="load from data", font=("Verdana", 19, "bold italic"), bd=0, command=loader, width=18, height=1, bg="#000000", fg="white", activebackground="#b8e6d2")
        dabuton.place(x=175,y=400)

        # Quand la fenêtre est fermée, réinitialiser loadopton
        loadopt.protocol("WM_DELETE_WINDOW", lambda: close_load_window(loadopt))

def close_load_window(window):
    global loadopton
    loadopton = False
    window.destroy()  # Ferme la fenêtre de chargement



def DeleteOption():
    global deleteopton
    if not deleteopton:  # Vérifie si une fenêtre de suppression n'est pas déjà ouverte
        deleteopton = True
        deleteopt = Toplevel(win)  # Crée une nouvelle fenêtre secondaire pour la suppression
        deleteopt.geometry("700x700")
        deleteopt.title('Delete Windows')
        deleteopt["bg"] = "#802412"
        deleteopt.resizable(height=False, width=False)

        # Crée un Label pour afficher la réponse
        delret.set("En attente de la réponse de l'Arduino...")
        TReponse = Label(deleteopt, text="Réponse de l'arduino :", font=("Verdana", 19, "bold italic"), fg="white", bg='#802412')
        TReponse.place(x=175,y=50)

        Reponse = Label(deleteopt, textvariable=delret, font=("Verdana", 15, ""), fg="white", bg='#802412')
        Reponse.pack(pady=150)

        Eraseall = Button(deleteopt, text="Erase all existence", font=("Verdana", 19, "bold italic"), bd=0, command=eraser, width=100, height=5, bg="#000000", fg="white", activebackground="#b8e6d2")
        Eraseall.pack(pady=25)

        # Quand la fenêtre est fermée, réinitialiser deleteopton
        deleteopt.protocol("WM_DELETE_WINDOW", lambda: close_delete_window(deleteopt))

def close_delete_window(window):
    global deleteopton
    deleteopton = False
    window.destroy()  # Ferme la fenêtre de suppression

# Titre principal
Titre = Label(win, text="Choose your options", font=("Verdana", 29, "bold italic"), fg="#d3deda", bg="#1c1726")
Titre.pack()

# Frame de sauvegarde
frameSave = Frame(win, width=300, height=150, background="#071a47", border=10)
frameSave.pack_propagate(False)
frameSave.place(x=200, y=75)

frameLoad = Frame(win, width=300, height=150, background="#214737", border=10)
frameLoad.pack_propagate(False)
frameLoad.place(x=200, y=250)

frameErase = Frame(win, width=300, height=150, background="#802412", border=10)
frameErase.pack_propagate(False)
frameErase.place(x=200, y=425)

# FRAME SAVE
Savetitle = Label(frameSave, text="Save Data", font=("Verdana", 19, "bold italic"), fg="white", bg='#071a47')
Savetitle.pack()

SaveButton = Button(frameSave, text="Enter Menu", font=("Verdana", 19, "bold italic"), bd=0, command=SaveOption, width=100, height=5, bg="#071330", fg="white", activebackground="#d0a4e0")
SaveButton.pack(pady=25)

# FRAME LOAD
Loadtitle = Label(frameLoad, text="Load Data", font=("Verdana", 19, "bold italic"), fg="white", bg='#214737')
Loadtitle.pack()

Loadbutton = Button(frameLoad, text="Enter Menu", font=("Verdana", 19, "bold italic"), bd=0, command=LoadOption, width=100, height=5, bg="#0b261b", fg="white", activebackground="#b8e6d2")
Loadbutton.pack(pady=25)

# FRAME ERASE
Erasetitle = Label(frameErase, text="Erase Data", font=("Verdana", 19, "bold italic"), fg="white", bg='#802412')
Erasetitle.pack()

Erasebutton = Button(frameErase, text="Enter Menu", font=("Verdana", 19, "bold italic"), bd=0, command=DeleteOption, width=100, height=5, bg="#400503", fg="white", activebackground="#c75f5b")
Erasebutton.pack(pady=25)

# Démarrer l'application

selectText = Label(win, text="put your serial port (ex /dev/ttyUSB0 or COM1) :", font=("Verdana", 14, "bold italic"), fg="#d3deda", bg="#1c1726")
selectText.place(y=600,x=10)
global comport
comport = StringVar()
selectCom = Entry(win,textvariable=comport,width=20)
selectCom.place(x=125,y=645)
setButton = Button(win, text="submit", font=("Verdana", 19, "bold italic"), bd=0, command=setport, width=10, height=1, bg="#000000", fg="white", activebackground="#b8e6d2")
setButton.place(x=300,y=635)
        
win.mainloop()
