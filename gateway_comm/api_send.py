import requests

url = "http://127.0.0.1:8000/radar_list/"

def send_to_api(data):
    try:
        payload = {}
        headers = {"Content-Type": "aplication/json"}

        response = requests.post(url=url, json=payload, headers=headers)

        if response.status_code == 201:
            print("OK")
        else:
            print("BAD")
    except Exception as e:
        print("WORSE")