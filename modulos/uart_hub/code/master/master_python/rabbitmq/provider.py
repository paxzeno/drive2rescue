import pika

from pika.exceptions import AMQPConnectionError, AMQPChannelError


class Provider:
    def __init__(self, host, port, username, password, queue):
        self.host = host
        self.port = port
        self.username = username
        self.password = password
        self.queue = queue

        self.conn = None
        self.channel = None

    def connect(self):
        try:
            self.conn = pika.BlockingConnection(
                pika.ConnectionParameters(host=self.host, port=self.port, connection_attempts=10, heartbeat_interval=120,
                                          credentials=pika.PlainCredentials(self.username, self.password)))
        except AMQPConnectionError as ex:
            print ex.message
            self.conn = None

        if self.conn is not None and self.conn.is_open:
            try:
                self.channel = self.conn.channel()
            except AMQPChannelError as ex:
                print ex.message
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
            print ex.message

    def is_connected(self):
        if self.conn is None or self.conn.is_open is not True:
            return False
        if self.channel is None or self.channel.is_open is not True:
            return False
        return True
