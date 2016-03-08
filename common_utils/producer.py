import config
from common_utils.connection import Connection

queueConfig = config.config_section_map('QueueConnection')
__request_queue = str(queueConfig['request_queue_name'])


def request(msg):
    send(msg, __request_queue)


def reply(msg):
    send(msg, msg.destination)


def send(msg, queue):
    Connection(queue).send(msg.to_queue())

