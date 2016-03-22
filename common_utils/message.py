import json
import time
import datetime


class Message:
    def __init__(self, origin=None, destination=None, operation=None, data=None):
        self.origin = self.__to_upper(origin)
        self.destination = self.__to_upper(destination)
        self.operation = self.__to_upper(operation)
        self.data = data
        self.timestamp = Message.__get_timestamp()

    def to_queue(self):
        if self.data is None:
            return json.dumps({"origin": self.origin, "destination": self.destination, "operation": self.operation})
        return json.dumps(
            {"origin": self.origin, "destination": self.destination, "operation": self.operation, "data": self.data})

    def from_queue(self, body):
        msg = json.loads(body)
        self.origin = self.__to_upper(msg.get('origin'))
        self.destination = self.__to_upper(msg.get('destination'))
        self.operation = self.__to_upper(msg.get('operation'))
        self.data = None if msg.get('data') is None else str(msg.get('data'))
        return self

    @staticmethod
    def __get_timestamp():
        return datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')

    @staticmethod
    def __to_upper(data):
        return None if data is None else str(data).upper()
