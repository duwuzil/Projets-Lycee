from tkinter import *
import serial
import re
import time

win = Tk()
win.geometry("500x500")
win.title('Data Logs')
win["bg"] = "black"
win.resizable(height=True,width=True)

Titre = Label(win,text="Choose your options")
Titre.pack()

Sec = Label(win,text="And goes out",font=("Verdana",29,"bold italic"),fg="white",bg="black")
Sec.place(x='0',y='250')


butt = Button(win,text="va tfer foutre")
butt.pack()

var = StringVar()
inp = Entry(win,textvariable=var)
inp.pack()
win.mainloop()
