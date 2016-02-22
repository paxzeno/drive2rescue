import uart_hub.module_communication as module_comm


def main():
    if module_comm.is_module_alive('GPS'):
        module_comm.get_gps_position('GPS')

    if module_comm.is_module_alive('3G'):
        module_comm.send_data_to_gsm('3G', 'not implemented')


if __name__ == '__main__': main()
