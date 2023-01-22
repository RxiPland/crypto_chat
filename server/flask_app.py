# python 3.11.1

import flask

import rsa
from cryptography.fernet import Fernet

import os
import shutil
import json
from urllib.parse import unquote
import hashlib
import uuid


app = flask.Flask(__name__)
working_dir = os.path.dirname(__file__).replace("\\", "/") + "/"


# generate AES key for server
server_aes_key = Fernet.generate_key()

# generate RSA keys for server
public_k, private_k = rsa.newkeys(2048)

# load bytes as server's AES key
symetric_key = Fernet(server_aes_key)

version = "crypto-chat v1.1.0"

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

if not os.path.exists(working_dir + "rooms"):
    os.mkdir(f"{working_dir}rooms")


@app.route('/')
def homepage():
    #return '<a href=\"https://github.com/RxiPland/crypto_chat\">https://github.com/RxiPland/crypto_chat</a>'

    return "Forbidden", 403


@app.route('/version', methods=["GET"])
def check_version():

    # specific user-agent is required
    if not "crypto-chat" in flask.request.user_agent.string:
        return "Forbidden", 403

    # desktop client will compare versions
    return version


@app.route('/get-public', methods=["GET"])
def get_public():

    # specific user-agent is required
    if not "crypto-chat" in flask.request.user_agent.string:
        return "Forbidden", 403
    
    # return server's Public key in hex
    return public_k.save_pkcs1().hex(), 200


@app.route('/get-symetric', methods=["POST"])
def get_symetric():
    # get server's symetric key

    """
    params: {'rsa_pem': "<user's RSA public key> in PEM"}

    response: {'data': '<RSA-encrypted-data> in hex'}
    <RSA-encrypted-data> = {'status_code': '<error code>', 'server_aes_key': '<plaintext server AES> in hex'}
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
        try:
            rsa_public_key = rsa.PublicKey.load_pkcs1(unquote(request_json["rsa_pem"]))
        
            data = {
                "status_code": "1",
                "server_aes_key": server_aes_key.hex()
            }

        except:
            print("Invalid RSA public key!")
            data = {
                "status_code": "5",
                "server_aes_key": ""
            }
        
        # encrypt data json with RSA public key
        encrypted_data = rsa.encrypt(str(data).encode(), rsa_public_key)

        return flask.jsonify({"data": encrypted_data.hex()}), 200

    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/create-room', methods=["POST"])
def create_room():
    # create new chat room

    """
    params: {'data': '<server-public-RSA-encrypted-data>'}
    <server-public-RSA-encrypted-data> = {'rsa_pem': "<user's RSA public key> in PEM", 'room_password_sha256': '<hashed password>'}

    response: {'data': '<user-public-RSA-encrypted-data> in hex'}
    <user-public-RSA-encrypted-data> = {'status_code': '<error code>', 'room_aes_key': '<plaintext room AES> in hex', 'room_id': '<32 chars hex string>'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403

        try:
            decrypted_data = rsa.decrypt(request_json["data"], priv_key=private_k)
        
        except:
            print("Invalid RSA encrypted data!")
            return "Forbidden", 403
        
        # keys 'rsa_pem' and 'room_password_sha256' must be in decrypted JSON
        if not "rsa_pem" in request_json.keys() or not "room_password_sha256" in request_json.keys():
            return "Forbidden", 403


        # load RSA public key from PEM
        try:
            user_rsa_public_key = rsa.PublicKey.load_pkcs1(unquote(decrypted_data["rsa_pem"]))

        except:
            print("Invalid RSA public key!")
            return "Forbidden", 403


        # create rooms folder in server's directory if not exists
        if not os.path.exists(working_dir + "rooms"):
            os.mkdir(f"{working_dir}rooms")

        rooms_path = working_dir + "rooms" + "/"

        # create 32 hex characters long ID
        room_id: str = uuid.uuid4().hex[:32]

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
            "room_id": room_id
        }

        # encrypt data json with RSA public key
        encrypted_data = rsa.encrypt(str(data).encode(), user_rsa_public_key)

        return flask.jsonify({"data": encrypted_data.hex()}), 200

    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/change-password', methods=["POST"])
def change_password():
    # change or set new password

    """
    params: {'data': '<encrypted-with-server-key> in hex'}
    <encrypted-with-room-key> = {'room_id': '<hex string (32)>', 'encrypted_room_password': '<encrypted-with-room-key> in hex'}

    response: {'data': '<encrypted-with-room-key> in hex'}
    <encrypted-with-room-key> = {'status_code': '<error code>'}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403


        decrypted_data: dict[str, str] = dict()

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


        # keys 'room_id' and 'encrypted_room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "encrypted_room_password" in decrypted_data.keys():
            return "Forbidden", 403

        room_id = decrypted_data["room_id"][:32]
        rooms_path = working_dir + "rooms" + "/"

        if not os.path.exists(rooms_path + room_id):
            # ID is wrong

            data = {
                "status_code": "4"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        # load AES key from room's folder
        with open(rooms_path + room_id + "/symetric_key", "rb") as f:
            room_key = f.read()

        try:
            # decrypt password with room's key

            room_key = Fernet(room_key)
            encrypted_password = bytes.fromhex(decrypted_data["encrypted_room_password"])

            # decrypt password
            room_password_sha256 = room_key.decrypt(encrypted_password)

            # overwrite password in file with new hashed password
            with open(rooms_path + room_id + "/password", "wb") as f:
                f.write(room_password_sha256)
        
        except:

            data = {
                "status_code": "5"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        data = {
            "status_code": "1"
        }

        data = str(data).encode()
        data = symetric_key.encrypt(data).hex()

        return flask.jsonify({"data": data}), 200


    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/join-room', methods=["POST"])
def join_room():
    """
    params: {'data': '<encrypted-with-room-key> in hex'}
    <encrypted-with-room-key> = {'room_id': '<random hex string (32)>', 'room_password': '<plaintext password from user>'}

    response: {'data': '<encrypted-data> in hex'}
    <encrypted-data> = {'status_code': '<error code>', 'room_aes_key': '<symetric key of room> in hex', 'messages_count': <int>}
    """

    try:
        # specific user-agent is required
        if not "crypto-chat" in flask.request.user_agent.string:
            return "Forbidden", 403

        request_json: dict = flask.request.get_json()

        # key 'data' must be in JSON
        if not "data" in request_json.keys():
            return "Forbidden", 403


        decrypted_data: dict[str, str] = dict()

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


        decrypted_data = json.loads(decrypted_data)

        # keys 'room_id' and 'room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys():
            return "Forbidden", 403

        room_id_user: str = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(working_dir + "/rooms/" + room_id_user):

            # wrong room ID
            data = {
                "status_code": "4"
            }

        else:
            password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()

            with open(working_dir + "/rooms/" + room_id_user + "/password", "r") as f:
                password_file = f.read()

            if password_user_hash.strip() != password_file.strip():

                # wrong password
                data = {
                    "status_code": "3"
                }

            else:
                key_path = working_dir + "/rooms/" + room_id_user + "/symetric_key"

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


                    messages_count_path = working_dir + "/rooms/" + room_id_user + "/messages_count"
                    if os.path.exists(messages_count_path):
                        with open(messages_count_path, "r") as f:
                            messages_count_server = f.read()
                    
                    else:
                        messages_count_server = "0"

                    # success
                    data = {
                        "status_code": "1",
                        "room_aes_key": aes_key.hex(),
                        "messages_count": messages_count_server
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

        decrypted_data: dict[str, str] = dict()

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
        if not os.path.exists(working_dir + "/rooms/" + room_id):

            # wrong room ID (room was deleted by someone else)
            data = {
                "status_code": "4"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()
        password_file_path = working_dir + "/rooms/" + room_id + "/password"


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
        with open(working_dir + "/rooms/" + room_id + "/messages", "r") as f:
            rows = f.readlines()

        # add new message
        rows.append(message + "\n")
        rows_count = len(rows)

        # max 100 messages will be stored
        while rows_count > 100:
            rows.pop(0)
            rows_count -= 1

        # write encrypted messages back to file
        with open(working_dir + "/rooms/" + room_id + "/messages", "w") as f:
            f.writelines(rows)


        messages_count_path = working_dir + "/rooms/" + room_id + "/messages_count"

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
    <AES-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password': '<plaintext password>', 'user_messages_count': <int>}

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


        decrypted_data: dict[str, str] = json.loads(decrypted_data)

        # keys 'room_id', 'room_password' and 'user_messages_count' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys() or not "user_messages_count" in decrypted_data.keys():
            return "Forbidden", 403


        room_id = decrypted_data["room_id"]

        # room folder in server's directory must exist
        if not os.path.exists(working_dir + "/rooms/" + room_id):

            # wrong room ID (room was deleted by someone else)
            data = {
                "status_code": "4"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200


        password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()
        password_file_path = working_dir + "/rooms/" + room_id + "/password"


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
        messages_count_path = working_dir + "/rooms/" + room_id + "/messages_count"

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
            with open(working_dir + "/rooms/" + room_id + "/messages", "r") as f:
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


        data = {
            "status_code": "1",
            "server_messages_count": messages_count_server,
            "skipped_messages": skipped_messages,
            "messages": messages_to_send
        }

        data = str(data).encode()
        data = symetric_key.encrypt(data).hex()

        return flask.jsonify({"data": data}), 200


    except Exception as e:
        print(e)
        return str(e), 403


@app.route('/delete-room', methods=["POST"])
def delete_room():
    """
    params: {'data': '<AES-encrypted-data> in hex'}
    <AES-encrypted-data> = {'room_id': '<hex string (32)>', 'room_password': '<plaintext password>'}

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

         # keys 'room_id' and 'room_password' must be in decrypted JSON
        if not "room_id" in decrypted_data.keys() or not "room_password" in decrypted_data.keys():
            return "Forbidden", 403

        room_id = decrypted_data["room_id"]

        password_user_hash: str = hashlib.sha256(decrypted_data["room_password"].encode()).hexdigest()
        password_file_path = working_dir + "/rooms/" + room_id + "/password"


        if os.path.exists(password_file_path):
            with open(password_file_path, "r") as f:
                password_file_hash = f.read()

        if password_user_hash.strip() != password_file_hash.strip():

            # wrong password (should never happen)
            data = {
                "status_code": "3"
            }

            data = str(data).encode()
            data = symetric_key.encrypt(data).hex()

            return flask.jsonify({"data": data}), 200

        try:
            shutil.rmtree(working_dir + "/rooms/" + room_id)

            data = {
                "status_code": "4"
            }

        except:
            data = {
                "status_code": "6"
            }

            
        data = str(data).encode()
        data = symetric_key.encrypt(data).hex()

        return flask.jsonify({"data": data}), 200


    except Exception as e:
        print(e)
        return str(e), 403


if __name__ == "__main__":
    app.run(debug=True, port=5000)
