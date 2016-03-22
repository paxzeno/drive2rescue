import serial
import time
from common_utils.message import Message
import common_utils.config as config

__CONFIG__ = config.config_section_map('Uart')

__SERIAL__ = None
__PORT__ = str(__CONFIG__['port'])
__BAUD__ = int(__CONFIG__['baud'])
__DIVIDER__ = str(__CONFIG__['divider'])
__RESPONSE_TIMEOUT__ = int(__CONFIG__['response_timeout'])


def set_timeout(value):
    global __RESPONSE_TIMEOUT__
    __RESPONSE_TIMEOUT__ = int(value)


def get_timeout():
    global __RESPONSE_TIMEOUT__
    return __RESPONSE_TIMEOUT__


def start_serial():
    global __SERIAL__, __PORT__, __BAUD__

    try:
        if __SERIAL__ is None:
            __SERIAL__ = serial.Serial(__PORT__, __BAUD__, timeout=1)

        if not __SERIAL__.isOpen():
            __SERIAL__.open()

        return True
    except Exception, error:
        __SERIAL__ = None
        print "Start serial error:"
        print error.message

    return False


def write_to_serial(msg):
    global __SERIAL__
    __SERIAL__.flushInput()
    __SERIAL__.flushOutput()
    __SERIAL__.write(msg)


# wait for message until it timeout or until something gets out
def wait_for_response():
    global __SERIAL__, __RESPONSE_TIMEOUT__, __DIVIDER__

    if not start_serial():  # start communication
        return Message("CONNECTION ERROR", "CONNECTION ERROR", "CONNECTION ERROR", "CONNECTION ERROR")

    response = ""
    start_time = time.time()

    while True:
        response += __SERIAL__.read()

        if '\r\n' in response:
            return get_response(response)
        elif (time.time() - start_time) >= __RESPONSE_TIMEOUT__:
            return Message("TIMEOUT", "TIMEOUT", "TIMEOUT", "TIMEOUT")


# build the message to be send via serial channel
# example: MASTER|GPS|AYT
def build_request(origin, destination, operation, data=None):
    global __DIVIDER__

    msg = ""
    msg += origin
    msg += __DIVIDER__
    msg += destination
    msg += __DIVIDER__
    msg += operation

    if data is not None:
        msg += __DIVIDER__
        msg += data

    msg += "\r\n"

    return msg


# read serial return message and converts the message in an array [ORIGIN, DESTINATION, OPERATION, DATA]
def get_response(msg):
    global __DIVIDER__

    parts = msg.replace("\r\n", "").split(__DIVIDER__)

    if len(parts) is 4:
        return Message(parts[0], parts[1], parts[2], parts[3])
    return Message("ERROR", "ERROR", "ERROR", "ERROR")
