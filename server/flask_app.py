# python 3.11.1

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
# délka prefixu: 25
# délka hesla: 100
# délka zprávy: 

if not os.path.exists(app_dir + "/rooms"):
    os.system(f"cd {app_dir} & mkdir rooms")

@app.route('/')
def hello_world():
    return '<a href=\"https://github.com/RxiPland/crypto_chat\">https://github.com/RxiPland/crypto_chat</a>'


@app.route('/version', methods=["GET"])
def check_version():

    # specific user-agent is required
    if not "crypto-chat" in flask.request.user_agent.string:
        return "Forbidden", 403
    
    # desktop client will compare versions
    return version


@app.route('/get-key', methods=["POST"])
def get_key():
    # params: {'rsa_pem': "<user's RSA public key in PEM>"}

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'rsa_pem' must be in JSON
        if not "rsa_pem" in request_json.keys():
            return "Forbidden", 403

        # load RSA public key from PEM
        rsa_public_key = rsa.PublicKey.load_pkcs1(unquote(request_json["rsa_pem"]))
        

        # encrypt server's AES key
        encrypted_aes = rsa.encrypt(server_aes_key, rsa_public_key)

        data = {
            "aes_key": encrypted_aes.hex()
        }

        return flask.jsonify(data)
    
    except Exception as e:
        print(e)
        return e, 403


@app.route('/create-room', methods=["POST"])
def create_room():
    # params: {'data': '<AES-encrypted-data> in hex'}
    # <AES-encrypted-data> = {'room_id': '<random hex string (32)>', 'room_password_sha256': '<hashed password from user>'}
    #
    # response: {'data': '<encrypted-data> in hex'}
    # <encrypted-data> = {'message': '<success message>', 'aes_key': '<symetric key of room>'}

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403

        # load bytes as server's AES key
        symetric_key = Fernet(server_aes_key)

        # decrypt data from request
        decrypted_data: dict = json.loads(symetric_key.decrypt(bytes.fromhex(request_json["data"])))


        # keys 'room_id' and 'room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys():
            return "Forbidden", 403

        # rooms folder in server's directory
        if not os.path.exists(app_dir + "/rooms"):
            os.system(f"cd {app_dir} & mkdir rooms")
        
        rooms_path = app_dir + "rooms" + "/"

        room_id: str = decrypted_data["room_id"]
        password: str = decrypted_data["room_password"]

        # create folder with random HEX string (room_id)
        os.system(f"cd {rooms_path} & mkdir " + room_id)

        # save password to file (in plain)
        with open(rooms_path + room_id + "/password", "w") as f:
            f.write(password)

        # generate AES key for room
        key = Fernet.generate_key()

        # save AES key to file
        with open(rooms_path + room_id + "/symetric_key", "wb") as f:
            f.write(key)


        # prepare data for return
        data = {
            "room_aes_key": symetric_key.encrypt(key).hex()
        }

        return flask.jsonify(data)

    except Exception as e:
        print(e)
        return e, 403


@app.route('/join-room', methods=["POST"])
def join_room():
    # params: {'data': '<AES-encrypted-data> in hex'}
    # <AES-encrypted-data> = {'room_id': '<random hex string (32)>', 'room_password_sha256': '<hashed password from user>'}
    #
    # response: {'data': '<encrypted-data> in hex'}
    # <encrypted-data> = {'message': '<success message>', 'aes_key': '<symetric key of room>'}
    pass
"""
    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403

        # load bytes as server's AES key
        symetric_key = Fernet(server_aes_key)

        # decrypt data from request
        decrypted_data: dict = json.loads(symetric_key.decrypt(bytes.fromhex(request_json["data"])))


        # keys 'room_id' and 'room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys():
            return "Forbidden", 403

        room_id_user: str = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(app_dir + "/rooms/" + room_id_user):
            return "Not Found", 404

        password_user: str = decrypted_data["room_password"]

        with open(app_dir + "/rooms/" + room_id_user + "/password", "r")

"""

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