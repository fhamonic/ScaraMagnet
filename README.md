# ScaraMagnet

This repository contains the Arduino script and python interface to controll a modified [PyBot SCARA](https://www.printables.com/fr/model/39958-arduino-robotic-arm-open-source-python-control-app) with an electromagnetic gripper.
Once the arduino_sketch is uploaded to your board, the python file scara_robot.py contains an interface object taking the port path and baudrate of the Arduino serial connection and exposing three methods:
- moveTo(x,y) : move the arm to the point x,y in [this coordinate system](error404)
- grab() : the arm lowers until touching the object to grab (or the table) activates the electromagnet and returns to its original height
- release() : same as grab but disables the electromagnet instead

# How to

Instal the python dependencies:

    pip install pyserial

Find the Arduino and webcam USB ports paths:

    ./list_usb_ports.sh

Assuming the Arduino location is '/dev/ttyACM0', set the appropriate permissions:

    sudo chmod a+rw /dev/ttyACM0

Upload the arduino skecth with Arduino IDE or with the command:

    arduino-CLI upload ./arduino_skecth --port /dev/ttyACM0

Run the python test interface:

    python interactive_position_control.py /dev/ttyACM0
