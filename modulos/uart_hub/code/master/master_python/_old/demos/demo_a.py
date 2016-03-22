import time
import serial

ser = serial.Serial('COM8', 9600, timeout=1)


if not ser.isOpen():
    ser.open()

ser.write("PI:GPS:AYT\r\n")

response = ""

while True:
    response += ser.read(1)

    if '\n' in response:
        print response
        response = ""
