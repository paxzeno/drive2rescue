from pika import ConnectionParameters
from pika import BlockingConnection
from pika import PlainCredentials
from pika import BasicProperties
import common_utils.config as config
from pika.exceptions import AMQPConnectionError, AMQPChannelError
import inspect


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
            self.connection = BlockingConnection(
                ConnectionParameters(host=self.host, port=self.port, connection_attempts=self.retries,
                                     credentials=PlainCredentials(self.username, self.password)))
        except AMQPConnectionError as ex:
            print ex.message
            self.channel = None

        if self.connection is not None and self.connection.is_open:
            try:
                self.connection.process_data_events()
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
                                          properties=BasicProperties(delivery_mode=1)):
                send_result = True
            self.channel.close()
            self.connection.close()

            return send_result
        except Exception as ex:
            print ex.message

    def listen(self, on_response, timeout=10):
        try:
            self.__get_channel()
            self.channel.basic_qos(prefetch_count=1)
            self.channel.basic_consume(on_response, self.queue)
            while Connection.__getresponse(on_response) is None:
                print('waiting for reply')
                self.connection.process_data_events(timeout)
                # self.channel.basic_get(queue=self.queue)
                method = Connection.__getmethod(on_response);
                if method is not None:
                    # inspect.getcallargs(on_response, 1, 2, 3, 4)
                    self.channel.basic_ack(method.delivery_tag)
                    print('response received, delivery tag: ' + str(method.delivery_tag))
        except Exception as ex:
            print ex.message
        finally:
            if self.channel is not None:
                self.channel.close()
            if self.connection is not None:
                self.connection.close()

    # FIXME find a more elegant way to do this.
    @staticmethod
    def __getmethod(on_response):
        return inspect.getmembers(on_response)[15][1].method

    # FIXME find a more elegant way to do this.
    @staticmethod
    def __getresponse(on_response):
        return inspect.getmembers(on_response)[15][1].response

    def listener(self, on_response):
        try:
            self.__get_channel()
            self.channel.basic_qos(prefetch_count=1)
            self.channel.basic_consume(on_response, queue=self.queue)
            print(" [x] Awaiting RPC requests")
            self.channel.start_consuming()
            print(" consumed")
        except Exception as ex:
            print ex.message
        finally:
            if self.channel is not None:
                self.channel.close()
            if self.connection is not None:
                self.connection.close()
