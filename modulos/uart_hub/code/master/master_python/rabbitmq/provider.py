import pika

from pika.exceptions import AMQPConnectionError, AMQPChannelError


class Provider:
    def __init__(self, host, port, queue):
        self.host = host
        self.port = port
        self.queue = queue

        self.conn = None
        self.channel = None

    def connect(self):
        try:
            self.conn = pika.BlockingConnection(pika.ConnectionParameters(host=self.host, port=self.port))
        except AMQPConnectionError as ex:
            print ex
            self.conn = None

        if self.conn is not None and self.conn.is_open:
            try:
                self.channel = self.conn.channel()
            except AMQPChannelError as ex:
                print ex
                self.channel = None

            if self.channel is not None and self.channel.is_open:
                self.channel.queue_declare(queue=self.queue, durable=True, exclusive=False, auto_delete=False)

    def disconnect(self):
        if self.channel is not None:
            self.channel.close()

        if self.conn is not None:
            self.conn.close()

    def send(self, msg):
        try:
            if self.channel.basic_publish(exchange='', routing_key=self.queue, body=msg,
                                          properties=pika.BasicProperties(delivery_mode=1)):
                return True
            return False
        except Exception as ex:
            print ex

    def is_connected(self):
        if self.conn is None or self.conn.is_open is not True:
            return False
        if self.channel is None or self.channel.is_open is not True:
            return False
        return True
