import config
import common_utils.connection as connection

queueConfig = config.config_section_map('QueueConnection')
__request_queue = str(queueConfig['request_queue_name'])


def request(msg):
    send(msg, __request_queue)


def reply(msg):
    send(msg, msg.destination)


def send(msg, queue):
    connection.Connection(queue).send(msg.to_queue())

