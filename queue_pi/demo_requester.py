from common_utils.message import Message
import common_utils.producer as producer
import common_utils.consumer as consumer


def request(body):
    msg_request = Message("master_queue", "GPS", "Operation", body)
    producer.request(msg_request)
    consumer.reply(on_response)


def reply(body):
    msg = Message("GPS", "master_queue", "Operation", body)
    producer.reply(msg)


def send_queue(body, queue_name):
    msg = Message("GPS", "master_queue", "Operation", body)
    producer.send(msg, queue_name)


def on_response(ch, method, props, body):
    print("message received: " + str(body))
