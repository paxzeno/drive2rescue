import serial_talk as talk
from common_utils.message import Message


class OperationManager:
    def __init__(self, msg):
        self.msg = msg

    def select(self):
        method_name = '_' + str(self.msg.destination).lower() + "_" + str(self.msg.operation).lower()

        method = getattr(self, method_name, lambda: "_unknown_error")
        return method()

    def _3g_send_data(self):
        """ send someting http://XXXXX/operation/xxxx """
        return generic_uart_comm(self.msg) if is_module_alive(self.msg) else self.__not_alive_error()

    def _gps_wai(self):
        """ get gps position data=>$GPGLL,3939.53990,N,00822.03619,W,122202.00,A,D*7C """
        return generic_uart_comm(self.msg) if is_module_alive(self.msg) else self.__not_alive_error()

    def _3g_set_apn(self):
        """ ...|data|net2.vodafone.pt;; """
        return generic_uart_comm(self.msg) if is_module_alive(self.msg) else self.__not_alive_error()

    def __unknown_error(self):
        """ unknown error message """
        return Message(self.msg.destination, self.msg.origin, self.msg.operation, "UNKNOWN ERROR")

    def __not_alive_error(self):
        """ not alive error message """
        return Message(self.msg.destination, self.msg.origin, self.msg.operation, "NOT ALIVE")


def generic_uart_comm(msg):
    """ generic modules communication
    :param msg: Message Object
    :return: response message
    """

    # start serial connection
    talk.start_serial()

    # build message MASTER|GPS|AYT
    message = talk.build_request(msg.origin, msg.destination, msg.operation, msg.data)

    # send data to serial channel
    talk.write_to_serial(message)

    # wait for response
    response = talk.wait_for_response()

    if response.data == 'TIMEOUT' or response.data == 'ERROR':
        response.origin = msg.destination
        response.destination = msg.origin
        response.operation = msg.operation

    return response


def is_module_alive(msg):
    """ check if module is capable to communicate with the master.
    :param msg: Message Object
    """
    current_timeout = talk.get_timeout()

    # set lower timeout for AYT operation
    talk.set_timeout(2)

    response = generic_uart_comm(Message(msg.origin, msg.destination, 'AYT'))

    # put old timeout again
    talk.set_timeout(current_timeout)

    if msg.destination == response.origin and 'AYT' == response.operation and 'OK' == response.data:
        return True

    return False
