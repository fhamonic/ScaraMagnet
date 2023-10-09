from braccio import Braccio
import sys
import time
from tkinter import *

if len(sys.argv) < 2:
    raise ValueError("Missing parameters: <arduino_port>")

arm = Braccio(sys.argv[1])
arm.home()

root = Tk()


def make_slider(name, bounds):
    label = Label(root, text=name)
    label.pack()
    slider = Scale(root, from_=bounds[0],
                   to=bounds[1], orient=HORIZONTAL, length=200)
    slider.pack()
    return slider


sliders = [make_slider(name, bounds) for (name, bounds) in zip(
    Braccio.axis_names, Braccio.axis_angle_bounds)]


def reset_angles():
    for (slider, angle) in zip(sliders, [0, 90, 90, 90, 90, 73]):
        slider.set(angle)


speed = make_slider("speed", (0, 200))

Scale(root, from_=0, to=200, orient=HORIZONTAL)
speed.pack()


def write_angles():
    arm.move_to([slider.get() for slider in sliders], speed.get())


Button(root, text='Reset', command=reset_angles).pack()
Button(root, text='Write', command=write_angles).pack()

reset_angles()
write_angles()

root.mainloop()
