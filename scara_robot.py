from serial import Serial
import time


class ScaraRobot:
    def __init__(self, path="/dev/ttyACM0", baud_rate=115200, timeout=5):
        self.serial = Serial(path, baud_rate, timeout=timeout)
        self.serial.readlines()  # wait for the arduino to wake up and empty the input buffer

    def move_to(self, x, y):
        angle_string = "M {x} {y}\n".format(
            x=x, y=y)
        self.serial.write(angle_string.encode())
        
    def grab(self):
        self.serial.write("G\n".encode())

    def release(self):
        self.serial.write("R\n".encode())
        
    def home(self):
        self.serial.write("H\n".encode())
