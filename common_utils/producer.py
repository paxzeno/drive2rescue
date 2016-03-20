import config
from common_utils.connection import Connection

queueConfig = config.config_section_map('QueueConnection')
__request_queue = str(queueConfig['request_queue_name'])


def request(msg):
    """ send message to request queue
    :param msg: msg to queue
    """
    send(msg, __request_queue)


def reply(msg):
    """ send message to destination
    :param msg: message to queue
    """
    send(msg, msg.destination)


def send(msg, queue):
    """
    :param msg: send message to custom queue
    :param queue: queue name
    """
    Connection(queue).send(msg.to_queue())

