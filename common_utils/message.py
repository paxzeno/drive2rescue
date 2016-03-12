import time
import datetime


class Message:
    def __init__(self, origin, destination, operation, body):
        self.origin = origin
        self.destination = destination
        self.operation = operation
        self.body = body
        self.timestamp = Message.__get_timestamp()

    def to_queue(self):
        return "origin:" + self.origin + " destination:" + self.destination + " operation:" + self.operation + \
               " body:" + self.body + " timestamp:" + self.timestamp

    @staticmethod
    def __get_timestamp():
        return datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
