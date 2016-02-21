import config
import gzip

compress = config.config_section_map('CompressMessages')['compress']


# message = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vivamus nec laoreet odio. Curabitur sed imperdiet lorem, non finibus arcu. In viverra at est eget iaculis. Cras et scelerisque metus, vel consequat tellus. Nunc et rhoncus elit. Aenean viverra luctus quam. Nunc luctus ut diam eu dapibus. Quisque vitae mi egestas, fringilla dolor feugiat, mollis lectus. Curabitur libero metus, eleifend sit amet odio at, eleifend tincidunt turpis. Nullam in libero justo. In dictum ornare ex, quis interdum augue vehicula id. Curabitur in lobortis lectus, eu placerat nibh. Aenean eu malesuada ipsum. Proin quis est in ante tincidunt rhoncus. Duis pulvinar nunc et consectetur condimentum. Interdum et malesuada fames ac ante ipsum primis in faucibus. Suspendisse egestas velit quis dolor tincidunt viverra. Sed viverra iaculis velit, et tempor diam auctor at. Sed at nulla est. Aliquam imperdiet luctus ligula. Mauris sodales nulla faucibus nulla lacinia molestie. Mauris vel neque in urna sollicitudin metus.'

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
