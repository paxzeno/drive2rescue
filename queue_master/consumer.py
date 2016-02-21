import pika
import gzip
import common_utils.config as config

queueConfig = config.config_section_map('QueueConnection')
__url = queueConfig['server_url']
__port = int(queueConfig['port'])

connection = pika.BlockingConnection(pika.ConnectionParameters(
    __url, port=__port))
channel = connection.channel()
channel.queue_declare(queue='101')


def callback(ch, method, properties, body):
    # only after python 3.2 :(
    # file_content = gzip.GzipFile(fileobj=body, mode='rb').read()

    with open("message.gz", "wb") as write_message:
        write_message.write(body)
    with gzip.open('message.gz', 'rb') as f:
        file_content = f.read()
    print(" [x] Received %r" % file_content)


channel.basic_consume(callback,
                      queue='101',
                      no_ack=True)
print(' [*] Waiting for messages. To exit press CTRL+C')
channel.start_consuming()
