import api_send as send
import read_serial as read
import time

if __name__ == "__main__":
    while True:
        data = read.read_from_serial()
        if data:
            send.send_to_api(data)
        time.sleep(1)