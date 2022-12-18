import flask

import rsa
from cryptography.fernet import Fernet

import os
import uuid

app = flask.Flask(__name__)
app_dir = os.path.dirname(__file__) + "/"

version = "crypto-chat v1.0.0"


@app.route('/')
def hello_world():
    return '<a href=\"https://github.com/RxiPland/crypto_chat\">https://github.com/RxiPland/crypto_chat</a>'


@app.route('/test', methods=["GET"])
def test():
    # test compatibility with app
    return version


@app.route('/create-room', methods=["POST"])
def create_room():
    # create room's file with random HEX name
    # generate AES key for room

    room_name = uuid.uuid4().hex
    os.system("mkdir " + "\"" + app_dir + room_name + "\"")

    key = Fernet.generate_key()

    with open(app_dir + room_name + "/symetric_key", "wb") as f:
        f.write(key)
    
    return room_name, 200


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