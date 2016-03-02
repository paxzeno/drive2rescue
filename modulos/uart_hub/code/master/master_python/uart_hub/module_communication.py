import serial_talk as talk


class OperationManager:
    def __init__(self, msg):
        self.origin = str(msg.get('origin')).upper()
        self.destination = str(msg.get('destination')).upper()
        self.operation = str(msg.get('operation')).upper()
        self.data = str(msg.get('data'))

    def select(self):
        method_name = '_' + str(self.destination).lower() + "_" + str(self.operation).lower()
        # build method
        method = getattr(self, method_name, lambda: "_unknown_error")
        return method()

    # send someting http://XXXXX/operation/xxxx
    def _3g_send_data(self):
        if is_module_alive(self.origin, self.destination):
            return generic_uart_comm(self.origin, self.destination, self.operation, self.data)
        else:
            return {"origin": self.destination, "destination": self.origin, "operation": self.operation,
                    "data": "NOT ALIVE"}

    # get gps position data=>$GPGLL,3939.53990,N,00822.03619,W,122202.00,A,D*7C
    def _gps_wai(self):
        if is_module_alive(self.origin, self.destination):
            return generic_uart_comm(self.origin, self.destination, self.operation)
        else:
            return {"origin": self.destination, "destination": self.origin, "operation": self.operation,
                    "data": "NOT ALIVE"}

    # ...|data|net2.vodafone.pt;;
    def _3g_set_apn(self):
        if is_module_alive(self.origin, self.destination):
            return generic_uart_comm(self.origin, self.destination, self.operation, self.data)
        else:
            return {"origin": self.destination, "destination": self.origin, "operation": self.operation,
                    "data": "NOT ALIVE"}

    # input message with errors
    def _unknown_error(self):
        return {"origin": self.destination, "destination": self.origin, "operation": self.operation,
                "data": "UNKNOWN ERROR"}


# generic modules communication
def generic_uart_comm(origin, destination, operation, data=None):
    # start serial connection
    talk.start_serial()

    # build message MASTER|GPS|AYT
    msg = talk.build_request(origin, destination, operation, data)

    # send data to serial channel
    talk.write_to_serial(msg)

    # wait for response
    response = talk.wait_for_response()

    if response['data'] == 'TIMEOUT' or response['data'] == 'ERROR':
        response['origin'] = destination
        response['destination'] = origin
        response['operation'] = operation

    return response


# check if module is capable to communicate with the master.
def is_module_alive(origin, destination):
    current_timeout = talk.get_timeout()

    # set lower timeout for AYT operation
    talk.set_timeout(2)

    response = generic_uart_comm(origin, destination, 'AYT')

    # put old timeout again
    talk.set_timeout(current_timeout)

    if destination == response['origin'] and 'AYT' == response['operation'] and 'OK' == response['data']:
        return True

    return False
