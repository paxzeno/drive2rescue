import serial_talk as talk


# check if module is capable to communicate with the master.
def is_module_alive(module):
    # start serial connection
    talk.start_serial()

    # build message MASTER|GPS|AYT
    msg = talk.build_request(module, 'AYT')

    # send data to serial channel
    talk.write_to_serial(msg)

    # wait for response
    response = talk.wait_for_response()

    if module == response['origin'] and 'AYT' == response['operation'] and 'OK' == response['data']:
        return True

    return False


#  get gps position data=>$GPGLL,3939.53990,N,00822.03619,W,122202.00,A,D*7C
def get_gps_position(module):
    # start serial connection
    talk.start_serial()

    # build message MASTER|GPS|AYT
    msg = talk.build_request(module, 'WAI')

    # send data to serial channel
    talk.write_to_serial(msg)

    # wait for response
    response = talk.wait_for_response()

    print response['origin']
    print response['destination']
    print response['operation']
    print response['data']


def send_data_to_gsm(module, data):
    # start serial connection
    talk.start_serial()

    # build message MASTER|GPS|AYT
    msg = talk.build_request(module, 'SEND_DATA', data)

    # send data to serial channel
    talk.write_to_serial(msg)

    # wait for response
    response = talk.wait_for_response()

    print response['origin']
    print response['destination']
    print response['operation']
    print response['data']