import pika
import common_utils.config as config
from pika.exceptions import AMQPConnectionError, AMQPChannelError


class Connection:
    def __init__(self, queue):
        queue_config = config.config_section_map('QueueConnection')
        self.host = str(queue_config['server_url'])
        self.port = int(queue_config['port'])
        self.retries = int(queue_config['retries'])
        self.username = str(queue_config['username'])
        self.password = str(queue_config['password'])

        self.connection = None
        self.channel = None
        self.queue = queue

    def __get_channel(self):
        try:
            self.connection = pika.BlockingConnection(
                pika.ConnectionParameters(host=self.host, port=self.port, connection_attempts=self.retries,
                                          credentials=pika.PlainCredentials(self.username, self.password)))
        except AMQPConnectionError as ex:
            print ex.message
            self.channel = None

        if self.connection is not None and self.connection.is_open:
            try:
                self.channel = self.connection.channel()
            except AMQPChannelError as ex:
                print ex.message
                self.channel = None
        if self.channel is not None and self.channel.is_open:
            self.channel.queue_declare(queue=self.queue, durable=True, exclusive=False, auto_delete=False)

    def send(self, msg):
        try:
            self.__get_channel()
            send_result = False
            if self.channel.basic_publish(exchange='', routing_key=self.queue, body=msg,
                                          properties=pika.BasicProperties(delivery_mode=1)):
                 send_result = True

            self.channel.close()
            self.connection.close()

            return send_result
        except Exception as ex:
            print ex.message
