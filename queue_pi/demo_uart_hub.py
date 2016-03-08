from common_utils.message import Message
import common_utils.producer as producer
import common_utils.consumer as consumer


def request():
    msg_reply = consumer.listen(on_response)
    print("message received: " + str(msg_reply))


def on_response(ch, method, props, body):
    print("on response: " + body)
    producer.reply(Message("GPS", "master_queue", "coordinates", "this is a experimental message"))


def main():
    request()


if __name__ == "__main__":
    main()
