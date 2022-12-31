# python 3.11.1

import rsa
from cryptography.fernet import Fernet

import sys
import os
import os.path
import tempfile
import uuid

app_dir = os.path.dirname(__file__).replace("\\", "/") + "/"
operations = "[generate_rsa, generate_aes, decrypt_rsa, encrypt_aes_server, decrypt_aes_server, sha256]"

def main():
    # operations -> [generate_rsa, decrypt_rsa, encrypt_aes, decrypt_aes]
    # arguments -> [key_size: int, b64_text: bytes, b64_text: bytes]

    argv = sys.argv[1:]
    length = len(argv)

    for i in range(length):
        argv[i] = argv[i].strip()
    

    if length == 0:
        raise Exception(f"Operation is missing! {operations}")
    
    operation = argv[0].lower()


    if not os.path.exists(app_dir + "temp"):
        os.system(f"cd {app_dir} & mkdir temp")
    
    if operation == "generate_rsa":
        # generate RSA key pairs (public & private)
        # generate random room_id
        # save them to files in Users/<username>/AppData/Local/Temp/{room_id}/


        # check if there is second argument
        if length >= 2:
            
            # check if string is an decimal
            if not argv[1].isdecimal():
                raise Exception("Size of RSA key must be decimal! [2048, 4096]")
            
            elif not argv[1] in ["2048", "4096"]:
                raise Exception("Size must be [2048, 4096] !")

            else:
                
                # generate random hex id (32)
                room_id = uuid.uuid4().hex
                temp_dir = tempfile.gettempdir()

                room_id_folder = temp_dir + f"\\{room_id}"

                if not os.path.exists(room_id_folder):
                    os.system("mkdir " + room_id_folder)

                # save id to file
                with open(app_dir + "temp/hex_id", "w") as f:
                    f.write(room_id)

                # generate keys
                public_k, private_k = rsa.newkeys(int(argv[1]))

                # save to file
                with open(room_id_folder + "\\public_key.pem", "wb") as f:
                    f.write(public_k.save_pkcs1())

                # save to file
                with open(room_id_folder + "\\private_key.pem", "wb") as f:
                    f.write(private_k.save_pkcs1())

        else:
            raise Exception("Size of RSA key is missing! [2048, 4096]")
   

    elif operation == "decrypt_rsa":
        # decrypt RSA cipher to obtain symetric key (AES)
        # save AES to file

        if length >= 2:

            room_id = argv[1]
            room_id_folder = tempfile.gettempdir() + f"\\{room_id}"

            with open(room_id_folder + "\\symetric_key_server", "r") as f:
                aes_crypt = f.read()
                aes_crypt = bytes.fromhex(aes_crypt)

            with open(room_id_folder + "\\private_key.pem", "rb") as f:
                private_k = rsa.PrivateKey.load_pkcs1(f.read())

            aes_plain = rsa.decrypt(crypto=aes_crypt, priv_key=private_k)

            with open(room_id_folder + "\\symetric_key_server", "wb") as f:
                f.write(aes_plain)

        else:
            raise Exception("Room ID missing!")


    elif operation == "encrypt_aes_server":
        # encrypt user's input with server's key and save to encrypted_message file

        if length >= 2:

            if length >= 3:

                room_id = argv[1]
                room_id_folder = tempfile.gettempdir() + f"\\{room_id}"
            
                message_plain = bytes.fromhex(argv[2])

                with open(room_id_folder + "\\symetric_key_server", "rb") as f:
                    symetric_key = Fernet(f.read())

                message_crypt = symetric_key.encrypt(data=message_plain)

                with open(room_id_folder + "\\encrypted_message", "w") as f:
                    f.write(message_crypt.hex())

            else:
                raise Exception("Plain text missing!")

        else:
            raise Exception("Room ID missing!")

    elif operation == "decrypt_aes_server":
        # open encrypted_message file and decrypt it's content with server symetric key
        # save decrypted content to decrypted_message file

        if length >= 2:

            room_id = argv[1]
            room_id_folder = tempfile.gettempdir() + f"\\{room_id}"

            with open(room_id_folder + "\\encrypted_message", "r") as f:
                message_crypt = bytes.fromhex(f.read())
                
            with open(room_id_folder + "\\symetric_key_server", "rb") as f:
                symetric_key = Fernet(f.read())

            try:
                message_plain = symetric_key.decrypt(message_crypt)

                with open(room_id_folder + "\\decrypted_message", "wb") as f:
                    f.write(message_plain)

            except:
                with open(room_id_folder + "\\decrypted_message", "wb") as f:
                    f.write("")
        
        else:
            raise Exception("Room ID missing!")


    elif operation == "encrypt_aes_room":
        # encrypt user's input with room's key and save to encrypted_message file

        if length >= 2:

            if length >= 3:

                room_id = argv[1]
                room_id_folder = tempfile.gettempdir() + f"\\{room_id}"
            
                message_plain = argv[2].encode("utf-8")

                with open(room_id_folder + "\\symetric_key_room", "rb") as f:
                    symetric_key = Fernet(f.read())

                message_crypt = symetric_key.encrypt(data=message_plain)

                with open(room_id_folder + "\\encrypted_message", "w") as f:
                    f.write(message_crypt.hex())

            else:
                raise Exception("Plain text missing!")

        else:
            raise Exception("Room ID missing!")


    elif operation == "decrypt_aes_room":
        # open encrypted_message file and decrypt it's content with room symetric key
        # save decrypted content to decrypted_message file

        if length >= 2:

            room_id = argv[1]
            room_id_folder = tempfile.gettempdir() + f"\\{room_id}"


            with open(room_id_folder + "\\encrypted_message", "r") as f:
                message_crypt = bytes.fromhex(f.read())

            with open(room_id_folder + "\\symetric_key_room", "rb") as f:
                symetric_key = Fernet(f.read())

            try:
                message_plain = symetric_key.decrypt(message_crypt)

                with open(room_id_folder + "\\decrypted_message", "wb") as f:
                    f.write(message_plain)

            except:
                with open(room_id_folder + "\\decrypted_message", "wb") as f:
                    f.write("")
        
        else:
            raise Exception("Room ID missing!")

    else:
        raise Exception(f"Invalid operation! {operations}")
    

main()
