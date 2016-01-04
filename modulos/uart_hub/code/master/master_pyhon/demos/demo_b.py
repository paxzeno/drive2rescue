import serial
import time

global_ser = None
global_hostname = "MASTER"
global_divider = ":"
global_thread_lock = None
global_wait_for_response_timeout = 10


######################################################
# BASE OPERATION
######################################################

######################################################
def open_serial():
    global global_ser
    if not global_ser.isOpen():
        global_ser.open()


######################################################
def write_to_serial(msg):
    global global_ser
    global_ser.write(msg)


######################################################
def wait_for_response():
    global global_ser, global_wait_for_response_timeout
    response = ""

    start_time = time.time()

    while True:
        response += global_ser.read()

        if '\r\n' in response:
            return response
        elif (time.time() - start_time) >= global_wait_for_response_timeout:
            return "TIMEOUT:TIMEOUT:TIMEOUT:TIMEOUT"


######################################################
# build message to be send to other modules
def build_output_message(module_name, operation):
    global global_hostname

    msg = ""
    msg += global_hostname
    msg += ":"
    msg += module_name
    msg += ":"
    msg += operation
    msg += "\r\n"

    return msg


######################################################
# GPS:MASTER:AYT:OK => just get [GPS, MASTER, AYT, OK]
def get_response_parts(msg):
    global global_divider
    msg_parts = msg.replace("\r\n", "").split(global_divider)

    if len(msg_parts) == 4:
        return msg_parts

    return ["ERROR", "ERROR", "ERROR", "ERROR"]


######################################################
# send hello to module and wait for a response.
def check_if_module_is_alive(module_name):
    # open connection if not open
    open_serial()

    # send data
    write_to_serial(build_output_message(module_name, 'AYT'))

    # wait for something
    response_msg = wait_for_response()

    # convert to message parts
    msg_parts = get_response_parts(response_msg)

    if module_name == msg_parts[0] and 'AYT' == msg_parts[2] and 'OK' == msg_parts[3]:
        return True

    return False


######################################################
# get gps raw data
def get_gps(module_name):
    # open connection if not open
    open_serial()

    # send data
    write_to_serial(build_output_message(module_name, 'WAI'))

    # wait for something
    response_msg = wait_for_response()

    # convert to message parts
    msg_parts = get_response_parts(response_msg)

    print msg_parts[0]
    print msg_parts[1]
    print msg_parts[2]
    print msg_parts[3]


######################################################
def main():
    global global_ser
    global_ser = serial.Serial('COM8', 9600, timeout=1)
    open_serial()  # open connection if not open

    if check_if_module_is_alive('GPS'):
        get_gps('GPS')


if __name__ == '__main__': main()
