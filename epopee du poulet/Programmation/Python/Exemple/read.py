
#!/usr/bin/env python3
import serial
import time
import os

port_serie = '/dev/ttyACM0'  
baudrate = 9600  

try:
    
    serial = serial.Serial(port_serie, baudrate, timeout=1)
    print("Connexion à l'Arduino :",port_serie,baudrate," bauds.")
    
    
    time.sleep(2)
    
    while True:
        if serial.in_waiting > 0:
            
            data = serial.readline().decode('utf-8').strip()  
            print("data:",data)
            if data == "D":
                os.system("clear")
        time.sleep(1.1)
except KeyboardInterrupt:
    print("Arrêt du script par l'utilisateur.")
finally:
    serial.close()
    print("Port série fermé.")
