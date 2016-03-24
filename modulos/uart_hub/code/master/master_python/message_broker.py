import time
import common_utils.consumer as consumer
import common_utils.producer as producer

from common_utils.message import Message
from uart_hub.operation_manager import OperationManager

__HOST__ = 'farma-abaco.no-ip.org'
__PORT__ = 5672
__QUEUE__ = 'requests'
__USER__ = 'xyon'
__PASS__ = 'fenix0'


def on_event(ch, method, properties, body):
    try:
        queue_msg = Message().from_queue(body)
        print "[x] request: ", queue_msg.to_queue()

        response_msg = OperationManager(queue_msg).select()
        producer.reply(response_msg)

        # 100 ms of wait to end communication
        time.sleep(5 / 10)  # pode ir para o fim para nao dar timeout no gajo que fez o pedido.

        print "[x] response: ", response_msg.to_queue()
        print

    except Exception as ex:
        print "[x] error: ", ex

    finally:
        ch.basic_ack(delivery_tag=method.delivery_tag)


def main(title):
    print title
    # c = consumer.Consumer(__HOST__, __PORT__, __USER__, __PASS__, __QUEUE__)
    # c.run_with_callback(on_event)
    consumer.listen(on_event)

    # restart
    main("restart")


if __name__ == '__main__': main("start")
