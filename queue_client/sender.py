import pika
import common_utils.config as config
import message_utils
#import pika.spec.BasicProperties as properties

carConfig = config.config_section_map('MyCar')
queueConfig = config.config_section_map('QueueConnection')
__url = queueConfig['server_url']
__port = int(queueConfig['port'])
car_id = carConfig['vehicle_id']


def send(message):
    byte_message = message_utils.prepare_message(message)
    connection = pika.BlockingConnection(pika.ConnectionParameters(__url, port=__port))
    channel = connection.channel()
    channel.queue_declare(queue=car_id)
    channel.basic_publish(exchange='',
                          routing_key=car_id,
                          body=byte_message)
    print " [x] Sent to queue[%s] message: %s" % (car_id, message)
    connection.close()
