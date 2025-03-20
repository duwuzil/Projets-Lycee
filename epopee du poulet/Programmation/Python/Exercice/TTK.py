from tkinter import *

win = Tk()
win.geometry("500x500")
win.title('Data Logs')
win["bg"] = "black"
win.resizable(height=True,width=True)

Titre = Label(win,text="Data logings goes here")
Titre.pack(pady=100)

Sec = Label(win,text="And goes out",font=("Verdana",29,"bold italic"),fg="white",bg="black")
Sec.place(x='0',y='250')



win.mainloop()