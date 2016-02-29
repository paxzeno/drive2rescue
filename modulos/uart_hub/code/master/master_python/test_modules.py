import json

import rabbitmq.provider as provider


def main():
    host = 'farma-abaco.no-ip.org'
    port = 5672
    queue = 'requests'

    p = provider.Provider(host, port, queue)
    p.connect()
    for i in range(1, 10):
        p.send(json.dumps({"origin": "MASTER", "destination": "GPS", "operation": "WAI", "data": str(i)}))
    p.disconnect()


if __name__ == '__main__': main()
