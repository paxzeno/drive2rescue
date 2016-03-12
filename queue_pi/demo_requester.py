from common_utils.message import Message
import common_utils.producer as producer
import common_utils.consumer as consumer


class Master:
    def __init__(self):
        print('init master')
        self.response = None
        self.processed = True
        self.method = None

    def request(self, body):
        msg_request = Message("zeno_master_queue", "GPS", "Operation", body)
        producer.request(msg_request)
        consumer.reply(self.on_response)

    @staticmethod
    def reply(body):
        msg = Message("GPS", "zeno_master_queue", "Operation", body)
        producer.reply(msg)

    @staticmethod
    def send_queue(body, queue_name):
        msg = Message("GPS", "zeno_master_queue", "Operation", body)
        producer.send(msg, queue_name)

    def on_response(self, ch, method, props, body):
        print("message received: " + str(body))
        self.response = body
        self.method = method


def main():
    Master().request('xXx : 21:35')


if __name__ == "__main__":
    main()
