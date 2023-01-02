# python 3.11.1

import flask

import rsa
from cryptography.fernet import Fernet

import os
import json
from urllib.parse import unquote
import hashlib


app = flask.Flask(__name__)
app_dir = os.path.dirname(__file__) + "/"

# generate AES key for server
server_aes_key = Fernet.generate_key()

# load bytes as server's AES key
symetric_key = Fernet(server_aes_key)

version = "crypto-chat v1.0.0"

"""
Status codes:
1 = ok
2 = file with symetric key not found
3 = wrong password
4 = wrong room ID
5 = wrong symetric key
"""

# max:
# délka jména: 25
# délka prefixu: 25
# délka hesla: 100
# délka zprávy: 
# počet zpráv: 100

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
    """
    params: {'rsa_pem': "<user's RSA public key> in PEM"}

    response: {'status_code': '<error code>', 'server_aes_key': '<encrypted server's AES key by public key> in hex'}
    """

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
            "status_code": "1",
            "server_aes_key": encrypted_aes.hex()
        }

        return flask.jsonify(data)
    
    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/create-room', methods=["POST"])
def create_room():
    """
    params: {'data': '<AES-encrypted-data> in hex'}
    <AES-encrypted-data> = {'room_id': '<random hex string (32)>', 'room_password_sha256': '<hashed password from user>'}
    
    response: {'data': '<encrypted-data> in hex'}
    <encrypted-data> = {'status_code': '<error code>', 'room_aes_key': '<symetric key of room> in hex'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403

        # decrypt data from request
        try:
            decrypted_data = symetric_key.decrypt(bytes.fromhex(request_json["data"]))
        
        except:
            
            # wrong symetric key
            data = {
                "status_code": "5",
                "room_aes_key": "None"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200

        decrypted_data: dict = json.loads(decrypted_data)

        # keys 'room_id' and 'room_password_sha256' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password_sha256" in decrypted_data.keys():
            return "Forbidden", 403

        # rooms folder in server's directory
        if not os.path.exists(app_dir + "/rooms"):
            os.system(f"cd {app_dir} & mkdir rooms")
        
        rooms_path = app_dir + "rooms" + "/"

        room_id: str = decrypted_data["room_id"]
        password: str = decrypted_data["room_password_sha256"]

        # create folder with random HEX string (room_id)
        os.system(f"cd {rooms_path} & mkdir " + room_id)

        # save password to file (already sha256 hash)
        with open(rooms_path + room_id + "/password", "w") as f:
            f.write(password)

        # create blank txt file for storing encrypted chat messages (only 100)
        with open(rooms_path + room_id + "/messages", "w") as f:
            f.write("")

        # create txt for storing count of recieved messages (not only 100)
        with open(rooms_path + room_id + "/messages_count", "w") as f:
            f.write("0")
            

        # generate AES key for room
        key = Fernet.generate_key()

        # save AES key to file
        with open(rooms_path + room_id + "/symetric_key", "wb") as f:
            f.write(key)


        # prepare data for return
        data = {
            "status_code": "1",
            "room_aes_key": key.hex()
        }

        # encrypt json
        data = str(data).encode()
        data = symetric_key.encrypt(data).hex()

        return flask.jsonify({"data": data}), 200

    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/join-room', methods=["POST"])
def join_room():
    """
    params: {'data': '<AES-encrypted-data> in hex'}
    <AES-encrypted-data> = {'room_id': '<random hex string (32)>', 'room_password': '<plaintext password from user>'}
    
    response: {'data': '<encrypted-data> in hex'}
    <encrypted-data> = {'status_code': '<error code>', 'room_aes_key': '<symetric key of room> in hex'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403


        # decrypt data from request
        try:
            decrypted_data = symetric_key.decrypt(bytes.fromhex(request_json["data"]))
        
        except:
            
            # wrong symetric key
            data = {
                "status_code": "5"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        decrypted_data: dict = json.loads(decrypted_data)

        # keys 'room_id' and 'room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys():
            return "Forbidden", 403

        room_id_user: str = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(app_dir + "/rooms/" + room_id_user):
            
            # wrong room ID
            data = {
                "status_code": "4"
            }

        else:
            password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()

            with open(app_dir + "/rooms/" + room_id_user + "/password", "r") as f:
                password_file = f.read()

            if password_user_hash.strip() != password_file.strip():
                
                # wrong password
                data = {
                    "status_code": "3"
                }

            else:
                key_path = app_dir + "/rooms/" + room_id_user + "/symetric_key"

                # AES key file must exist
                if not os.path.exists(key_path):

                    # symetric key file not found
                    data = {    
                        "status_code": "2"
                    }

                else:
                    # read room AES key from file
                    with open(key_path, "rb") as f:
                        aes_key = f.read()

                    # success
                    data = {
                        "status_code": "1",
                        "room_aes_key": aes_key.hex()
                    }


        data = str(data).encode()
        data = symetric_key.encrypt(data).hex()

        return flask.jsonify({"data": data}), 200


    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/send-message', methods=["POST"])
def send_message():
    """
    params: {'data': '<AES-encrypted-data> in hex'}
    <AES-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password': '<plain text password>', 'message': '<encrypted-message> with room symetric key in hex'}
    
    response: {'data': '<encrypted-data> in hex'}
    <encrypted-data> = {'status_code': '<error code>'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403


        # decrypt data from request
        try:
            decrypted_data = symetric_key.decrypt(bytes.fromhex(request_json["data"]))
        
        except:
            
            # wrong symetric key
            data = {
                "status_code": "5"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        decrypted_data: dict = json.loads(decrypted_data)

        # keys 'room_id', 'room_password' and 'message' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys() or not "message" in decrypted_data.keys():
            return "Forbidden", 403

        room_id = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(app_dir + "/rooms/" + room_id):
            
            # wrong room ID (room was deleted by someone else)
            data = {
                "status_code": "4"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()
        password_file_path = app_dir + "/rooms/" + room_id + "/password"


        if os.path.exists(password_file_path):
            with open(password_file_path, "r") as f:
                password_file_hash = f.read()
        
        else:
            # password file missing (should never happen)

            with open(password_file_path, "w") as f:
                f.write(password_user_hash)

            password_file_hash = password_user_hash


        if password_user_hash.strip() != password_file_hash.strip():
                
            # wrong password (should never happen)
            data = {
                "status_code": "3"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200

        message = decrypted_data["message"]

        # load all messages
        with open(app_dir + "/rooms/" + room_id + "/messages", "r") as f:
            rows = f.readlines()

        # add new message
        rows.append(message + "\n")
        rows_count = len(rows)

        # max 100 messages will be stored
        while rows_count > 100:
            rows.pop(0)
            rows_count -= 1

        # write encrypted messages back to file
        with open(app_dir + "/rooms/" + room_id + "/messages", "w") as f:
            f.writelines(rows)


        messages_count_path = app_dir + "/rooms/" + room_id + "/messages_count"

        # read value
        if os.path.exists(messages_count_path):
            with open(messages_count_path, "r") as f:
                messages_count = f.read()

                if messages_count.isdecimal():
                    messages_count = int(messages_count)
                
                else:
                    # (should never happen)
                    messages_count = 0
        else:
            messages_count = 0
        
        # increment value
        with open(messages_count_path, "w") as f:
            f.write(str(messages_count + 1))


        data = {
            "status_code": "1"
        }

        data = str(data).encode()
        data = symetric_key.encrypt(data).hex()

        return flask.jsonify({"data": data}), 200


    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/get-messages', methods=["POST"])
def get_messages():
    """
    params: {'data': '<AES-encrypted-data> in hex'}
    <AES-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password_sha256': '<hashed password>', 'user_messages_count': <int>}
    
    response: {'data': '<encrypted-data> in hex'}
    <encrypted-data> = {'status_code': '<error code>', 'server_messages_count': <int>, 'messages': [<encrypted with room key>, ...]}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403


        # decrypt data from request
        try:
            decrypted_data = symetric_key.decrypt(bytes.fromhex(request_json["data"]))
        
        except:
            
            # wrong symetric key
            data = {
                "status_code": "5"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        decrypted_data: dict = json.loads(decrypted_data)

        # keys 'room_id', 'room_password_sha256' and 'user_messages_count' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password_sha256" in decrypted_data.keys() or not "user_messages_count" in decrypted_data.keys():
            return "Forbidden", 403


        room_id = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(app_dir + "/rooms/" + room_id):
            
            # wrong room ID (room was deleted by someone else)
            data = {
                "status_code": "4"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        password_user_hash: str = decrypted_data["room_password_sha256"]
        password_file_path = app_dir + "/rooms/" + room_id + "/password"


        if os.path.exists(password_file_path):
            with open(password_file_path, "r") as f:
                password_file_hash = f.read()
        
        else:
            # password file missing (should never happen)

            with open(password_file_path, "w") as f:
                f.write(password_user_hash)

            password_file_hash = password_user_hash


        if password_user_hash.strip() != password_file_hash.strip():
                
            # wrong password (should never happen)
            data = {
                "status_code": "3"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200

        messages_count_user: int = decrypted_data["user_messages_count"]
        messages_count_path = app_dir + "/rooms/" + room_id + "/messages_count"
        
        if os.path.exists(messages_count_path):
            with open(messages_count_path, "r") as f:
                messages_count_server = f.read()

                if messages_count_server.isdecimal():
                    messages_count_server = int(messages_count_server)
                
                else:
                    # (should never happen)
                    messages_count_server = 0
        else:
            messages_count_server = 0

        
        if messages_count_user == 0:
            messages_to_send_count = 1
        else:
            messages_to_send_count: int = messages_count_server - messages_count_user
        
        messages_to_send: list = []

        if messages_to_send_count == 0:
            pass

        else:

            # load all messages
            with open(app_dir + "/rooms/" + room_id + "/messages", "r") as f:
                rows = f.readlines()

            if messages_to_send_count > 100:
                # max 100 messages
                messages_to_send = rows
                messages_to_send.insert(0, f"({messages_to_send_count-100} zpráv bylo přeskočeno) ...")

            else:
                rows = rows[::-1]

                for i in range(messages_to_send_count):
                    messages_to_send.append(rows[i])

                messages_to_send = messages_to_send[::-1]


        data = {
            "status_code": "1",
            "server_messages_count": messages_count_server,
            "messages": messages_to_send
        }

        data = str(data).encode()
        data = symetric_key.encrypt(data).hex()

        return flask.jsonify({"data": data}), 200


    except Exception as e:
        print(e)
        return str(e), 403


if __name__ == "__main__":
    app.run(debug=True, port=5000)
