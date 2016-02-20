import pika
import gzip

connection = pika.BlockingConnection(pika.ConnectionParameters(
    'localhost'))
channel = connection.channel()
channel.queue_declare(queue='hello')


def callback(ch, method, properties, body):
    #message_byte_array = bytearray(body)
    with open("message.gz", "wb") as write_message:
        write_message.write(body)
    with gzip.open('message.gz', 'rb') as f:
        file_content = f.read()
    print(" [x] Received %r" % file_content)


channel.basic_consume(callback,
                      queue='hello',
                      no_ack=True)
print(' [*] Waiting for messages. To exit press CTRL+C')
channel.start_consuming()
