from serial import Serial
import time


class Braccio:
    axis_names = ["base", "shoulder", "elbow", "wrist", "wristRot", "gripper"]
    axis_angle_bounds = [(0, 180), (15, 165), (0, 180),
                         (0, 180), (0, 180), (0, 73)]

    def __init__(self, path, baud_rate=115200, timeout=5):
        print("INIT")
        self.serial = Serial(path, baud_rate, timeout=timeout)
        self.serial.readlines()  # wait for the arduino to wake up and empty the input buffer

    def wait_and_handle_ack(self):
        response = self.serial.readline().decode().strip()
        if response != "OK":
            raise Exception("Arduino responded: '{}'".format(response))

    def power_on(self):
        print("POWER_ON")
        self.serial.write("1\n".encode())
        self.wait_and_handle_ack()

    def power_off(self):
        print("POWER_OFF")
        self.serial.write("0\n".encode())
        self.wait_and_handle_ack()

    @staticmethod
    def clamp_angles(angles):
        return [min(max_angle, max(min_angle, angle))
                for (angle, (min_angle, max_angle)) in zip(angles, Braccio.axis_angle_bounds)]

    def move_to(self, angles, speed=20):
        print("MOVE_TO {} {}".format(angles, speed))
        self.current_angles = Braccio.clamp_angles(angles)
        angle_string = "P {angles} {speed}\n".format(
            angles=' '.join([str(int(elem)) for elem in self.current_angles]), speed=speed)
        self.serial.write(angle_string.encode())
        self.wait_and_handle_ack()

    def get_current_angles(self):
        return self.current_angles.copy()

    def home(self, speed=20):
        print("HOME")
        home_angles = [0, 30, 0, 0, 90, 73]
        self.move_to(home_angles, speed)

    def shutdown(self, speed=50):
        print("SHUTDOWN")
        shutdown_angles = [0, 110, 20, 20, 90, 0]
        self.move_to(shutdown_angles, speed)
        self.power_off()
