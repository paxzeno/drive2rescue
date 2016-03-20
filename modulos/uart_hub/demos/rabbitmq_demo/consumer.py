import time
import pika
import json

from pika.exceptions import ConnectionClosed


class OperationManager:
    def __init__(self, msg):
        self.msg = msg

    def select(self):
        destination = self.msg.get('destination')
        operation = self.msg.get('operation')

        method_name = '_' + str(destination).lower() + "_" + str(operation).lower()
        method = getattr(self, method_name, lambda: "nothing")
        method()

    def _3g_send_data(self):
        print "[x] - send data to 3G ", self.msg.get('data')

    def _gps_wai(self):
        print "[x] - get position from gps ", self.msg.get('operation')


class Consumer:
    def __init__(self, host, port, queue, timeout=0):
        self.host = host
        self.port = port
        self.queue = queue
        self.timeout = timeout
        self.start_timeout = time.time()
        self.is_timeout = False

    def run_with_callback(self, callback):
        try:
            connection = pika.BlockingConnection(
                    pika.ConnectionParameters(host=self.host, port=self.port, connection_attempts=10,
                                              heartbeat_interval=10))

            channel = connection.channel()
            channel.queue_declare(queue=self.queue, durable=True, exclusive=False, auto_delete=False)

            channel.basic_consume(callback, queue=self.queue)
            channel.start_consuming()

            return True
        except Exception as ex:
            print ex
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
        if (time.time() - self.start_timeout) >= self.timeout:
            self.is_timeout = True
        return False


def on_event(ch, method, properties, body):
    try:
        op = OperationManager(json.loads(body))
        op.select()
    finally:
        ch.basic_ack(delivery_tag=method.delivery_tag)


def main():
    consumer = Consumer('farma-abaco.no-ip.org', 5672, 'requests', 10)
    # consumer.run_with_callback(on_event)
    print consumer.run_sync()


if __name__ == '__main__': main()
