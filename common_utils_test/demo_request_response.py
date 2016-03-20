import common_utils.producer as producer
import common_utils.consumer as consumer

from common_utils.message import Message


def request():
    msg_reply = consumer.listen(on_response)
    print("message received: " + str(msg_reply))


def on_response(ch, method, props, body):
    print("on response: " + body)
    ch.basic_ack(method.delivery_tag)
    # producer.reply(Message("GPS", "zeno_request_queue", "coordinates", "original message:1"))


def main():
    # producer.reply(Message("GPS", "zeno_master_queue", "coordinates", "original message:and this is the reply"))
    # producer.reply(Message("GPS", "zeno_request_queue", "coordinates", "original message:1"))
    print consumer.reply("zeno_request_queue")
    # request()


if __name__ == "__main__":
    main()
