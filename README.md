# python-braccio

Install prerequisites:

    pip install pyserial opencv-python

Find the Arduino and webcam USB ports paths:

    ./list_usb_ports.sh

Assuming the Arduino location is '/dev/ttyACM0', set the appropriate permissions:

    sudo chmod a+rw /dev/ttyACM0

Upload the arduino skecth with Arduino IDE or:

    arduino-CLI upload ./arduino_skecth --port /dev/ttyACM0

Run the python script:

    python test.py /dev/ttyACM0 /dev/video1