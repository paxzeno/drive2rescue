class Message:
    def __init__(self, origin, destination, operation, body):
        self.origin = origin
        self.destination = destination
        self.operation = operation
        self.body = body

    def to_queue(self):
        return "origin:" + self.origin + " destination:" + self.destination + " operation:" + self.operation +\
               " body:" + self.body
