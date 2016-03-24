import common_utils.consumer as consumer
import common_utils.producer as producer

from common_utils.message import Message

__QUEUE__ = "MASTER"
__APN_CONFIG_A = "net2.vodafone.pt;;"
__APN_CONFIG_B = "net2.vodafone.pt;;"

gps_info = Message(__QUEUE__, "GPS", "WAI")
set_new_config_a = Message(__QUEUE__, "3G", "SET_APN", __APN_CONFIG_A)
set_new_config_b = Message(__QUEUE__, "3G", "SET_APN", __APN_CONFIG_B)
send_data = Message(__QUEUE__, "3G", "SEND_DATA")  # data needs to be build


def main():
    # process GPS info

    producer.request(gps_info)
    print consumer.reply(__QUEUE__)

    # send msg
    # producer.request(send_data)
    # print consumer.reply(__QUEUE__, 60)

    # change config
    producer.request(set_new_config_a)
    print consumer.reply(__QUEUE__, 60)



if __name__ == '__main__': main()
