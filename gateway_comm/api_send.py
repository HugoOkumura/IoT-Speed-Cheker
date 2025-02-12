import requests

url = "http://127.0.0.1:8000/"

def send_to_api(data):
    try:
        # print(data)
        tempo = int(data[0])
        distancia = int(data[1])
        velocidade = int(data[2])

        if int(data[3]) == 0:
            limite = False
        else:
            limite = True

        payload = {"tempo":tempo, "distancia": distancia, "velocidade":velocidade, "limite":limite}
        print(payload)
        headers = {"Content-Type": "application/json"}

        response = requests.post(url=url, json=payload, headers=headers)

        if response.status_code == 201:
            print("OK")
        else:
            print("BAD")
    except Exception as e:
        print("WORSE")