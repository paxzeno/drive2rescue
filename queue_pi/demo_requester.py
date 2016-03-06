import common_utils.message as message
import common_utils.producer_ as producer


def request(body):
    msg = message.Message("master_queue", "GPS", "Operation", body)
    producer.request(msg)


def reply(body):
    msg = message.Message("GPS", "master_queue", "Operation", body)
    producer.reply(msg)


def send_queue(body, queue_name):
    msg = message.Message("GPS", "master_queue", "Operation", body)
    producer.send(msg, queue_name)
