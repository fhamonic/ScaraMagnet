from braccio import Braccio
import sys
import time
import math
from tkinter import *

if len(sys.argv) < 2:
    raise ValueError("Missing parameters: <arduino_port>")

arm = Braccio(sys.argv[1])
arm.home()

root = Tk()
Label(root, text="Base")
base_entry = Entry(root)
base_entry.pack()
Label(root, text="X")
x_entry = Entry(root)
x_entry.pack()
Label(root, text="Y")
y_entry = Entry(root)
y_entry.pack()

r1 = 125
r2 = 125
r3 = 190

def angles_to_pos(angles):
    O1 = math.radians(angles[1])
    O2 = math.radians(angles[2])
    O3 = math.radians(angles[3])
    x = r1 * math.sin(O1 - math.pi/2) + r2 * math.sin(O1 - O2) + r3 * math.sin(math.pi/2 + O1 - O2 - O3)
    y = 25 + r1 * math.cos(O1 - math.pi/2) + r2 * math.cos(O1 - O2) + r3 * math.cos(math.pi/2 + O1 - O2 - O3)
    return (x, y)


def dist(p1, p2):
    return math.sqrt((p2[0]-p1[0])**2 + (p2[1]-p1[1])**2)

def derivate_axis(axis_num, angles, target):
    error0 = dist(target, angles_to_pos(angles))
    angles[axis_num] += 0.1
    error1 = dist(target, angles_to_pos(angles))
    return error1 - error0

def derivate_shoulder(angles):
    O1 = math.radians(angles[1])
    O2 = math.radians(angles[2])
    O3 = math.radians(angles[3])
    return r1 * math.cos(O1 - math.pi/2) + r2 * math.cos(O1 - O2) + r3 * math.cos(math.pi/2 + O1 - O2 - O3)

def go():
    target = (float(x_entry.get()), float(y_entry.get()))
    current_angles = arm.get_current_angles()
    current_angles[0] = int(base_entry.get())
    arm.move_to(current_angles)
    current_angles = arm.get_current_angles()
    error = dist(target, angles_to_pos(current_angles))
    while error > 2:
        print("error = {}".format(error))
        d1 = derivate_axis(1, current_angles.copy(), target)
        d2 = derivate_axis(2, current_angles.copy(), target)
        d3 = derivate_axis(3, current_angles.copy(), target)

        current_angles[1] -= d1 * error / 10
        current_angles[2] -= d2 * error / 10
        current_angles[3] -= d3 * error / 10

        arm.move_to(current_angles)


        if [int(v) for v in current_angles] == arm.get_current_angles():
            break

        current_angles = arm.get_current_angles()
        error = dist(target, angles_to_pos(current_angles))
        

def open_grip():
    current_angles = arm.get_current_angles()
    current_angles[5] = 0
    arm.move_to(current_angles)

def close_grip():
    current_angles = arm.get_current_angles()
    current_angles[5] = 73
    arm.move_to(current_angles)


Button(root, text='Go', command=go).pack()
Button(root, text='Open Grip', command=open_grip).pack()
Button(root, text='Close Grip', command=close_grip).pack()
root.mainloop()
