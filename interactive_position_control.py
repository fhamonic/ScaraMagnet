from scara_robot import ScaraRobot
import sys
import time
import math
from tkinter import *

if len(sys.argv) < 2:
    raise ValueError("Missing parameters: <arduino_port>")

arm = ScaraRobot(sys.argv[1])

root = Tk()

Label(root, text="X:", anchor='w').grid(row=0, column=0)
x_entry = Entry(root)
x_entry.grid(row=0, column=1)
Label(root, text="Y:", anchor='w').grid(row=1, column=0)
y_entry = Entry(root)
y_entry.grid(row=1, column=1)

def moveTo():
    arm.move_to(float(x_entry.get()), float(y_entry.get()))

def home():
    arm.home()

def grab():
    arm.grab()

def release():
    arm.release()

Button(root, text='MoveTo', command=moveTo).grid(row=2, column=1)
Button(root, text='Home', command=home).grid(row=3, column=1)
Button(root, text='Grab', command=grab).grid(row=4, column=1)
Button(root, text='Release', command=release).grid(row=5, column=1)
root.mainloop()
