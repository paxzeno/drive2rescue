import config
from common_utils.connection import Connection

queueConfig = config.config_section_map('QueueConnection')
__request_queue = str(queueConfig['request_queue_name'])


def reply(queue, timeout=10):
    return Connection(queue).listen_sync(timeout)


def listen(on_response):
    return Connection(__request_queue).listener(on_response)
