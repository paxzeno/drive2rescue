import config
import gzip

compress = config.config_section_map('CompressMessages')['compress']


def prepare_message(message):
    if compress:
        with gzip.open('message.gz', 'wb') as write_message:
            write_message.write(message)
        with open("message.gz", "rb") as read_message:
            message_content = read_message.read()
            message_byte_array = bytearray(message_content)
            return str(message_byte_array)
    else:
        print("don't compress")
