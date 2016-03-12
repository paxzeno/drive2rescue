from common_utils.message import Message
import common_utils.producer as producer
import common_utils.consumer as consumer


def request():
    msg_reply = consumer.listen(on_response)
    print("message received: " + str(msg_reply))


def on_response(ch, method, props, body):
    print("on response: " + body)
    ch.basic_ack(method.delivery_tag)
    producer.reply(
        Message("GPS", "zeno_master_queue", "coordinates", "original message:" + body + " and this is the reply"))


def main():
    request()


if __name__ == "__main__":
    main()
