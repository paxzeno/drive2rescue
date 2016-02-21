import pika
import config
import message_utils

carConfig = config.config_section_map('MyCar')
queueConfig = config.config_section_map('QueueConnection')
url = queueConfig['server_url']
car_id = carConfig['vehicle_id']


def send(message):
    byte_message = message_utils.prepare_message(message)
    connection = pika.BlockingConnection(pika.ConnectionParameters(url))
    channel = connection.channel()
    channel.queue_declare(queue=car_id)
    channel.basic_publish(exchange='',
                          routing_key='hello',
                          body=byte_message)
    print " [x] Sent to queue[%s] message: %s" % (car_id, message)
    connection.close()
