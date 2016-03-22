import json

import rabbitmq.provider as provider


def main():
    host = 'farma-abaco.no-ip.org'
    port = 5672
    queue = 'requests'
    username = 'xyon'
    password = 'fenix0'

    p = provider.Provider(host, port, username, password, queue)
    p.connect()
    for i in range(0, 1):
        # p.send(json.dumps({"origin": "MASTER", "destination": "GPS", "operation": "WAI"}))
        # p.send(json.dumps({"origin": "MASTER", "destination": "GPS", "operation": "WAI", "data": "net2.vodafone.pt;aa;bb"}))

        # p.send(json.dumps({"origin": "MASTER", "destination": "3G", "operation": "SET_APN", "data": "net2.vodafone.pt;aa;bb"}))

        p.send(json.dumps({"origin": "MASTER", "destination": "3G", "operation": "SEND_DATA", "data": "http://www.m2msupport.net/m2msupport/test.php"}))

    p.disconnect()


if __name__ == '__main__': main()
