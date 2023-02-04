# python 3.11.1

import flask

import rsa
from cryptography.fernet import Fernet

import os
import shutil
import json
import hashlib


app = flask.Flask(__name__)
WORKING_DIR = os.path.dirname(__file__).replace("\\", "/") + "/"

# generate 2048 bit RSA keys for server
RSA_PUBLIC_K, RSA_PRIVATE_K = rsa.newkeys(2048)


VERSION = "crypto-chat v1.1.0"

"""
- Status codes:
1 = ok
2 = file with symetric key not found
3 = wrong password
4 = wrong room ID / successfull deletetion
5 = wrong symetric/RSA key
6 = other error

- Maximums:
username length: 25
prefix length: 25
password length: 100
message length: 100_000
number of stored messages (server): 100
"""

# create rooms folder in server directory
if not os.path.exists(WORKING_DIR + "rooms"):
    os.mkdir(f"{WORKING_DIR}rooms")


@app.route('/')
def homepage():

    return "Forbidden", 403


@app.route('/version', methods=["GET"])
def check_version():

    # specific user-agent is required
    if not "crypto-chat" in flask.request.user_agent.string:
        return "Forbidden", 403

    # desktop client will compare versions
    return VERSION


@app.route('/get-public', methods=["GET"])
def get_public():

    # specific user-agent is required
    if not "crypto-chat" in flask.request.user_agent.string:
        return "Forbidden", 403
    
    # return server's Public key in hex
    return RSA_PUBLIC_K.save_pkcs1().hex(), 200


@app.route('/create-room', methods=["POST"])
def create_room():
    # create new chat room

    """
    params: {'rsa_pem': '<user RSA public key> in PEM hex', 'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password_sha256': '<hashed password>'}

    response: {'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'status_code': '<error code>', 'room_aes_key': '<plaintext room AES> in hex'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # keys 'rsa_pem' and 'data_rsa' must be in JSON
        if not "rsa_pem" in request_json.keys() or not "data_rsa" in request_json.keys():
            return "Forbidden", 403


        # load user's RSA public key from PEM
        # decrypt data_rsa
        try:
            user_rsa_publickey = rsa.PublicKey.load_pkcs1(bytes.fromhex(request_json["rsa_pem"]))
            decrypted_data: dict[str, str] = json.loads(rsa.decrypt(bytes.fromhex(request_json["data_rsa"]), priv_key=RSA_PRIVATE_K))

        except:
            # Invalid RSA public key / encrypted data
            return "Forbidden", 403
        

        # keys 'room_id' and 'room_password_sha256' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password_sha256" in decrypted_data.keys():
            return "Forbidden", 403


        # create rooms folder in server's directory if not exists
        if not os.path.exists(WORKING_DIR + "rooms"):
            os.mkdir(f"{WORKING_DIR}rooms")

        rooms_path = WORKING_DIR + "rooms" + "/"
        room_id: str = decrypted_data["room_id"][:32]

        # create folder with random HEX string (room_id)
        os.mkdir(rooms_path + room_id)

        # create blank txt file for storing encrypted chat messages (only 100)
        with open(rooms_path + room_id + "/messages", "w") as f:
            f.write("")

        # create txt for storing count of recieved messages (not only 100)
        with open(rooms_path + room_id + "/messages_count", "w") as f:
            f.write("0")
        
        # write hashed password to file
        with open(rooms_path + room_id + "/password", "w") as f:
            f.write(decrypted_data["room_password_sha256"])


        # generate AES key for room
        key = Fernet.generate_key()

        # save AES key to file
        with open(rooms_path + room_id + "/symetric_key", "wb") as f:
            f.write(key)


        # prepare data for return
        data = {
            "status_code": "1",
            "room_aes_key": key.hex(),
        }

        # encrypt data json with RSA public key
        encrypted_data = rsa.encrypt(str(data).encode(), user_rsa_publickey)

        return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200

    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/change-password', methods=["POST"])
def change_password():
    # set new password for room

    """
    params: {'rsa_pem': '<user's RSA public key> in PEM hex', 'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'room_id': '<hex string (32)>', 'current_password': '<plaintext password>', 'new_password': '<plaintext password>'}

    response: {'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'status_code': '<error code>'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # keys 'rsa_pem' and 'data_rsa' must be in JSON
        if not "rsa_pem" in request_json.keys() or not "data_rsa" in request_json.keys():
            return "Forbidden", 403


        # load user's RSA public key from PEM
        # decrypt data_rsa
        try:
            user_rsa_publickey = rsa.PublicKey.load_pkcs1(bytes.fromhex(request_json["rsa_pem"]))
            decrypted_data: dict[str, str] = json.loads(rsa.decrypt(bytes.fromhex(request_json["data_rsa"]), priv_key=RSA_PRIVATE_K))

        except:
            # Invalid RSA public key / encrypted data
            return "Forbidden", 403


        # keys 'room_id' and 'encrypted_new_room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "current_password" in decrypted_data.keys() or not "new_password" in decrypted_data.keys():
            return "Forbidden", 403

        room_id = decrypted_data["room_id"][:32]
        rooms_path = WORKING_DIR + "rooms" + "/"

        if not os.path.exists(rooms_path + room_id):
            # ID is wrong

            data = {
                "status_code": "4"
            }

            encrypted_data = rsa.encrypt(str(data).encode(), user_rsa_publickey)

            return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200


        password_user_hash: str = hashlib.sha256(decrypted_data["current_password"].encode()).hexdigest()

        with open(rooms_path + room_id + "/password", "r") as f:
            password_file = f.read()

        if password_user_hash.strip() != password_file.strip():

            # wrong password
            data = {
                "status_code": "3"
            }

            encrypted_data = rsa.encrypt(str(data).encode(), user_rsa_publickey)
            
            return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200

        else:
            # save hashed new password to file
            with open(rooms_path + room_id + "/password", "w") as f:

                new_password_sha256 = hashlib.sha256(decrypted_data["new_password"].encode()).hexdigest()
                f.write(new_password_sha256)


        data = {
            "status_code": "1"
        }

        encrypted_data = rsa.encrypt(str(data).encode(), user_rsa_publickey)

        return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200


    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/join-room', methods=["POST"])
def join_room():
    # join existing room

    """
    params: {'rsa_pem': "<user's RSA public key> in PEM hex", 'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password': '<plaintext password from user>'}

    response: {'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'status_code': '<error code>', 'room_aes_key': '<plaintext room AES> in hex', 'messages_count': <int>}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # keys 'rsa_pem' and 'data_rsa' must be in JSON
        if not "rsa_pem" in request_json.keys() or not "data_rsa" in request_json.keys():
            return "Forbidden", 403


        # load user's RSA public key from PEM
        # decrypt data_rsa
        try:
            user_rsa_publickey = rsa.PublicKey.load_pkcs1(bytes.fromhex(request_json["rsa_pem"]))
            decrypted_data: dict[str, str] = json.loads(rsa.decrypt(bytes.fromhex(request_json["data_rsa"]), priv_key=RSA_PRIVATE_K))

        except:
            # Invalid RSA public key / encrypted data
            return "Forbidden", 403


        # keys 'room_id' and 'room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys():
            return "Forbidden", 403

        room_id_user: str = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(WORKING_DIR + "/rooms/" + room_id_user):

            # wrong room ID
            data_rsa = {
                "status_code": "4",
                "room_aes_key": None,
                "messages_count": None
            }

        else:
            password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()

            with open(WORKING_DIR + "/rooms/" + room_id_user + "/password", "r") as f:
                password_file = f.read()

            if password_user_hash.strip() != password_file.strip():

                # wrong password
                data_rsa = {
                    "status_code": "3",
                    "room_aes_key": None,
                    "messages_count": None
                }

            else:
                key_path = WORKING_DIR + "/rooms/" + room_id_user + "/symetric_key"

                # AES key file must exist
                if not os.path.exists(key_path):

                    # symetric key file not found
                    data_rsa = {
                        "status_code": "2",
                        "room_aes_key": None,
                        "messages_count": None
                    }

                else:
                    # read room AES key from file
                    with open(key_path, "rb") as f:
                        aes_key = f.read()


                    messages_count_path = WORKING_DIR + "/rooms/" + room_id_user + "/messages_count"
                    if os.path.exists(messages_count_path):
                        with open(messages_count_path, "r") as f:
                            messages_count_server = f.read()
                    
                    else:
                        messages_count_server = "0"

                    # success
                    data_rsa = {
                        "status_code": "1",
                        "room_aes_key": aes_key.hex(),
                        "messages_count": messages_count_server
                    }


        encrypted_data = rsa.encrypt(str(data_rsa).encode(), user_rsa_publickey)
            
        return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200


    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/send-message', methods=["POST"])
def send_message():
    # send message to existing room

    """
    params: {'rsa_pem': '<user RSA public key> in PEM', 'data_rsa': '<RSA-encrypted-data> in hex', 'data_aes': '<AES-encrypted-message> in hex'}
    <RSA-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password': '<plain text password>', 'symetric_key': <temp-AES-key-used-to-encrypt-data_aes> in hex}
    <AES-encrypted-message> = {'message': '<encrypted-with-room-AES-key> in hex'}

    response: {'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'status_code': '<error code>'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403


        request_json: dict = flask.request.get_json()

        # keys 'rsa_pem', 'data_rsa' and 'data_aes' must be in JSON
        if not "rsa_pem" in request_json.keys() or not "data_rsa" in request_json.keys() or not "data_aes" in request_json.keys():
            return "Forbidden", 403

        decrypted_data: dict[str, str] = dict()

        # load user's RSA public key from PEM
        # decrypt data_rsa
        try:
            user_rsa_publickey = rsa.PublicKey.load_pkcs1(bytes.fromhex(request_json["rsa_pem"]))
            decrypted_data: dict[str, str] = json.loads(rsa.decrypt(bytes.fromhex(request_json["data_rsa"]), priv_key=RSA_PRIVATE_K))

        except:
            # Invalid RSA public key / encrypted data
            return "Forbidden", 403


        # keys 'room_id', 'room_password' and 'symetric_key' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys() or not "symetric_key" in decrypted_data.keys():
            return "Forbidden", 403

        room_id = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(WORKING_DIR + "/rooms/" + room_id):

            # wrong room ID (room was deleted by someone else)
            data_rsa = {
                "status_code": "4"
            }

            encrypted_data = rsa.encrypt(str(data_rsa).encode(), user_rsa_publickey)
            
            return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200


        password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()

        password_file_path = WORKING_DIR + "/rooms/" + room_id + "/password"

        if os.path.exists(password_file_path):
            with open(password_file_path, "r") as f:
                password_file_hash = f.read()

        else:
            # password file missing (should never happen)
            password_file_hash = ""


        if password_user_hash.strip() != password_file_hash.strip():

            # wrong password
            data_rsa = {
                "status_code": "3"
            }

            encrypted_data = rsa.encrypt(str(data_rsa).encode(), user_rsa_publickey)
            
            return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200

        try:
            # load temporary symetric key and decrypt data_aes to obtain message

            temp_symetric_key = Fernet(bytes.fromhex(decrypted_data["symetric_key"]))

            decrypted_aes_data = temp_symetric_key.decrypt(bytes.fromhex(request_json["data_aes"]))
            message = json.loads(decrypted_aes_data)["message"]

        except:
            # wrong symetric key
            data_rsa = {
                "status_code": "5"
            }

            encrypted_data = rsa.encrypt(str(data_rsa).encode(), user_rsa_publickey)
            
            return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200


        # load all messages
        with open(WORKING_DIR + "/rooms/" + room_id + "/messages", "r") as f:
            rows = f.readlines()

        # add new message
        rows.append(message + "\n")
        rows_count = len(rows)

        # max 100 messages will be stored
        while rows_count > 100:
            rows.pop(0)
            rows_count -= 1

        # write encrypted messages back to file
        with open(WORKING_DIR + "/rooms/" + room_id + "/messages", "w") as f:
            f.writelines(rows)


        messages_count_path = WORKING_DIR + "/rooms/" + room_id + "/messages_count"

        # read value
        if os.path.exists(messages_count_path):
            with open(messages_count_path, "r") as f:
                messages_count = str(f.read())

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


        data_rsa = {
            "status_code": "1"
        }


        encrypted_data = rsa.encrypt(str(data_rsa).encode(), user_rsa_publickey)
        
        return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200


    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/get-messages', methods=["POST"])
def get_messages():
    # get new messages (max 100)

    """
    params: {'rsa_pem': '<user RSA public key> in PEM hex', 'data_rsa': '<RSA-encrypted-data> in hex'}
    <RSA-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password': '<plain text password>', 'user_messages_count': <int>}

    response: {'data_rsa': <RSA-encrypted-data> in hex, 'data_aes': <temp-AES-key-encrypted-data> in hex}
    <RSA-encrypted-data> = {'status_code': '<error code>', 'server_messages_count': <int>, 'skipped_messages': <int>, 'symetric_key': <temp-AES-key-for-json-encryption> in hex}
    <AES-encrypted-data> = {'messages': [<encrypted with room key>, ...]}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # keys 'rsa_pem' and 'data_rsa' must be in JSON
        if not "rsa_pem" in request_json.keys() or not "data_rsa" in request_json.keys():
            return "Forbidden", 403


        # load user's RSA public key from PEM
        # decrypt data_rsa
        try:
            user_rsa_publickey = rsa.PublicKey.load_pkcs1(bytes.fromhex(request_json["rsa_pem"]))
            decrypted_data: dict[str, str | int] = json.loads(rsa.decrypt(bytes.fromhex(request_json["data_rsa"]), priv_key=RSA_PRIVATE_K))

        except:
            # Invalid RSA public key / encrypted data
            return "Forbidden", 403


        # keys 'room_id', 'room_password' and 'user_messages_count' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys() or not "user_messages_count" in decrypted_data.keys():
            return "Forbidden", 403


        room_id = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(WORKING_DIR + "/rooms/" + room_id):

            # wrong room ID (room was deleted by someone else)
            data_rsa = {
                "status_code": "4",
                "server_messages_count": None,
                "skipped_messages": None,
                "symetric_key": None
            }

            encrypted_data = rsa.encrypt(str(data_rsa).encode(), user_rsa_publickey)
        
            return flask.jsonify({"data_rsa": encrypted_data.hex(), "data_aes": None}), 200


        password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()
        password_file_path = WORKING_DIR + "/rooms/" + room_id + "/password"


        if os.path.exists(password_file_path):
            with open(password_file_path, "r") as f:
                password_file_hash = f.read()

        else:
            # password file missing (should never happen)
            password_file_hash = ""


        if password_user_hash.strip() != password_file_hash.strip():

            # wrong password (should never happen)
            data_rsa = {
                "status_code": "3",
                "server_messages_count": None,
                "skipped_messages": None,
                "symetric_key": None
            }

            encrypted_data = rsa.encrypt(str(data_rsa).encode(), user_rsa_publickey)
        
            return flask.jsonify({"data_rsa": encrypted_data.hex(), "data_aes": None}), 200


        messages_count_user: int = decrypted_data["user_messages_count"]
        messages_count_path = WORKING_DIR + "/rooms/" + room_id + "/messages_count"

        if os.path.exists(messages_count_path):
            with open(messages_count_path, "r") as f:
                messages_count_server = str(f.read())

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
        skipped_messages: int = 0

        if messages_to_send_count == 0:
            pass

        else:

            # load all messages
            with open(WORKING_DIR + "/rooms/" + room_id + "/messages", "r") as f:
                rows = f.readlines()

            if messages_to_send_count > 100:
                # max 100 messages
                messages_to_send = rows
                skipped_messages = messages_to_send_count-100

            else:

                rows = rows[::-1]

                for i in range(messages_to_send_count):
                    messages_to_send.append(rows[i])

                messages_to_send = messages_to_send[::-1]


        temp_symetric_key = Fernet.generate_key()


        data_rsa = {
            "status_code": "1",
            "server_messages_count": messages_count_server,
            "skipped_messages": skipped_messages,
            "symetric_key": temp_symetric_key.hex()
        }
        encrypted_data_rsa = rsa.encrypt(str(data_rsa).encode(), user_rsa_publickey)


        data_aes = {
            "messages": messages_to_send
        }
        encrypted_data_aes = Fernet(temp_symetric_key).encrypt(str(data_aes).encode())
        
        return flask.jsonify({"data_rsa": encrypted_data_rsa.hex(), "data_aes": encrypted_data_aes.hex()}), 200


    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/delete-room', methods=["POST"])
def delete_room():
    # delete existing room (delete room's folder)

    """
    params: {'rsa_pem': '<user RSA public key> in PEM hex', 'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password': '<plaintext password>'}

    response: {'data_rsa': <RSA-encrypted-data> in hex}
    <RSA-encrypted-data> = {'status_code': '<error code>'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # keys 'rsa_pem' and 'data_rsa' must be in JSON
        if not "rsa_pem" in request_json.keys() or not "data_rsa" in request_json.keys():
            return "Forbidden", 403


        # load user's RSA public key from PEM
        # decrypt data_rsa
        try:
            user_rsa_publickey = rsa.PublicKey.load_pkcs1(bytes.fromhex(request_json["rsa_pem"]))
            decrypted_data: dict[str, str] = json.loads(rsa.decrypt(bytes.fromhex(request_json["data_rsa"]), priv_key=RSA_PRIVATE_K))

        except:
            # Invalid RSA public key / encrypted data
            return "Forbidden", 403


         # keys 'room_id' and 'room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys():
            return "Forbidden", 403

        room_id = decrypted_data["room_id"]

        password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()
        password_file_path = WORKING_DIR + "/rooms/" + room_id + "/password"


        if os.path.exists(password_file_path):
            with open(password_file_path, "r") as f:
                password_file_hash = f.read()

        if password_user_hash.strip() != password_file_hash.strip():

            # wrong password (should never happen)
            data = {
                "status_code": "3"
            }

            encrypted_data = rsa.encrypt(str(data).encode(), user_rsa_publickey)
        
            return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200


        try:
            # delete folder

            shutil.rmtree(WORKING_DIR + "/rooms/" + room_id)

            data = {
                "status_code": "4"
            }

        except:
            data = {
                "status_code": "6"
            }

            
        encrypted_data = rsa.encrypt(str(data).encode(), user_rsa_publickey)
        
        return flask.jsonify({"data_rsa": encrypted_data.hex()}), 200


    except Exception as e:
        print(e)
        return str(e), 403


if __name__ == "__main__":
    app.run(debug=True, port=5000)
