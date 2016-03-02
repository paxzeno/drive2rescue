import pika
import time

from pika.exceptions import ConnectionClosed


class Consumer:
    def __init__(self, host, port, username, password, queue, timeout=0):
        self.host = host
        self.port = port
        self.username = username
        self.password = password
        self.queue = queue
        self.timeout = timeout
        self.start_timeout = time.time()
        self.is_timeout = False

    def run_with_callback(self, callback):
        try:
            connection = pika.BlockingConnection(
                    pika.ConnectionParameters(host=self.host, port=self.port, connection_attempts=10,
                                              heartbeat_interval=120,
                                              credentials=pika.PlainCredentials(self.username, self.password)))

            channel = connection.channel()
            channel.queue_declare(queue=self.queue, durable=True, exclusive=False, auto_delete=False)

            channel.basic_consume(callback, queue=self.queue)
            channel.start_consuming()

            return True
        except Exception as ex:
            print ex.message
            return False

    def run_sync(self):
        output = None
        try:
            connection = pika.BlockingConnection(pika.ConnectionParameters(host=self.host, port=self.port))

            channel = connection.channel()
            channel.queue_declare(queue=self.queue, durable=True, exclusive=False, auto_delete=False)

            while True:
                result = channel.basic_get(queue=self.queue)
                if result[2] is not None:
                    channel.basic_ack(result[0].delivery_tag)
                    output = result[2]
                    break
                else:
                    time.sleep(1)
                    if self.stop_on_timeout():
                        break

            channel.close()
            connection.close()
        except ConnectionClosed as ex:
            time.sleep(1)
            if not self.stop_on_timeout():
                output = self.run_sync()
        return output

    def stop_on_timeout(self):
        if self.timeout == 0:
            return False

        if (time.time() - self.start_timeout) >= self.timeout:
            self.is_timeout = True
        return False


# example of callback
# def on_event(ch, method, properties, body):
#    try:
#        op = OperationManager(json.loads(body))
#        op.select()
#    finally:
#        ch.basic_ack(delivery_tag=method.delivery_tag)
