import flask

import rsa
from cryptography.fernet import Fernet

import os
import json
from urllib.parse import unquote


app = flask.Flask(__name__)
app_dir = os.path.dirname(__file__) + "/"

# generate AES key for server
server_aes_key = Fernet.generate_key()

version = "crypto-chat v1.0.0"

# max:
# délka jména: 25
# délka hesla: 100

@app.route('/')
def hello_world():
    return '<a href=\"https://github.com/RxiPland/crypto_chat\">https://github.com/RxiPland/crypto_chat</a>'


@app.route('/version', methods=["GET"])
def check_version():

    if not "crypto-chat" in flask.request.user_agent.string:
        return "Forbidden", 403
    
    # app will check compatibility
    return version


@app.route('/get-key', methods=["POST"])
def get_key():
    # params: rsa_pem

    if not "crypto-chat" in flask.request.user_agent.string:
        return "Forbidden", 403

    request_json: dict = flask.request.get_json()

    if not "rsa_pem" in request_json.keys():
        return "Forbidden", 403

    rsa_public_key = rsa.PublicKey.load_pkcs1(unquote(request_json["rsa_pem"]))
    encrypted_aes = rsa.encrypt(server_aes_key, rsa_public_key)

    data = {
        "aes_key": encrypted_aes.hex()
    }

    return flask.jsonify(data)


@app.route('/create-room', methods=["POST"])
def create_room():

    if not "crypto-chat" in flask.request.user_agent.string:
        return "Forbidden", 403

    request_json: dict = json.loads(flask.request.get_json())

    if not "room_id" in request_json.keys():
        return "Forbidden", 403

    room_id: str = request_json["room_id"]

    # create room's file with random HEX name
    os.system("mkdir " + "\"" + app_dir + room_id + "\"")

    # generate AES key for room and save it to file
    key = Fernet.generate_key()

    with open(app_dir + room_id + "/symetric_key", "wb") as f:
        f.write(key)


    # prepare data for return
    data = {
        "roomId": room_id
    }

    return flask.jsonify(data)


if __name__ == "__main__":
    app.run(debug=True, port=5000)



"""
@app.route('/crypt/<zprava>', methods=["GET"])
def sifrovat_zpravu(zprava: str):

    key = Fernet.generate_key()
    f = Fernet(key)
    zasifrovano = f.encrypt(zprava.encode())
    desifrovano = f.decrypt(zasifrovano)
    return "Původní zpráva: " + zprava + "<br><br>Vygenerovaný symetrický klíč: " + key.decode()  + "<br><br>Zašifrovaná zpráva: " + zasifrovano.decode() + "<br><br>Dešifrovaná zpráva: " + desifrovano.decode()
"""