import pika
import config

queueConfig = config.config_section_map('QueueConnection')
url = queueConfig['server_url']


def send(message):
    connection = pika.BlockingConnection(pika.ConnectionParameters(url))
    channel = connection.channel()
    channel.queue_declare(queue='hello')
    channel.basic_publish(exchange='',
                          routing_key='hello',
                          body=message)
    print(" [x] Sent 'Hello World!'")
    connection.close()
