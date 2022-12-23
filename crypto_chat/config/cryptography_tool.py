import rsa
from cryptography.fernet import Fernet
import base64

import sys
import os
import os.path
import tempfile
import uuid

app_dir = os.path.dirname(__file__).replace("\\", "/") + "/"

def main():

    # operations -> [generate_rsa, decrypt_rsa, encrypt_aes, decrypt_aes]
    # arguments -> [key_size: int, b64_text: bytes, b64_text: bytes]

    argv = sys.argv[1:]
    length = len(argv)

    for i in range(length):
        argv[i] = argv[i].strip()
    

    if length == 0:
        print("Chybí operace! [generate_rsa, generate_aes, decrypt_rsa, encrypt_aes, decrypt_aes]")
        return
    
    operation = argv[0].lower()


    if not os.path.exists(app_dir + "temp"):
        os.system(f"cd {app_dir} & mkdir temp")
    
    if operation == "generate_rsa":
        # generate RSA key pairs (public & private)
        # generate random room_id
        # save them to files in Users/.../AppData/Local/Temp/{room_id}/


        # check if there is second argument
        if length == 2:
            
            # check if string is an decimal
            if not argv[1].isdecimal():
                print("Chybí velikost RSA klíče v bitech! [2048, 4096]")
                return
            
            elif not argv[1] in ["2048", "4096"]:
                print("Velikost musí být z výběru! [2048, 4096]")
                return

            else:
                
                # generate random id
                room_id = uuid.uuid4().hex

                room_id_file = tempfile.gettempdir() + f"\\{room_id}"

                if not os.path.exists(room_id_file):
                    os.system(f"mkdir \"{room_id_file}\"")

                # save id to file
                with open(app_dir + "temp/hex_id", "w") as f:
                    f.write(room_id)

                # generate keys
                public_k, private_k = rsa.newkeys(int(argv[1]))

                # save to file
                with open(room_id_file + "\\public_key.pem", "wb") as f:
                    f.write(public_k.save_pkcs1())

                # save to file
                with open(room_id_file + "\\private_key.pem", "wb") as f:
                    f.write(private_k.save_pkcs1())

        else:
            print("Chybí velikost RSA klíče v bitech! [2048, 4096]")
            return
   

    elif operation == "decrypt_rsa":
        # decrypt RSA cipher to obtain symetric key (AES)
        # save AES to file

        if length == 2:

            room_id = argv[1]
            room_id_file = tempfile.gettempdir() + f"\\{room_id}"

            with open(room_id_file + "\\symetric_key_server", "r") as f:
                aes_crypt = f.read()
                aes_crypt = bytes.fromhex(aes_crypt)

            with open(room_id_file + "\\private_key.pem", "rb") as f:
                private_k = rsa.PrivateKey.load_pkcs1(f.read())

            aes_plain = rsa.decrypt(crypto=aes_crypt, priv_key=private_k)

            with open(room_id_file + "\\symetric_key_server", "wb") as f:
                f.write(aes_plain)

        else:

            print("Chybí ID místnosti!")
            return


    elif operation == "encrypt_aes":
        # encrypt user's input to encrypted_message file

        if length == 2:
        
            message_plain = argv[1].encode("utf-8")

            with open(room_id_file + "\\symetric_key_server", "rb") as f:
                symetric_key = Fernet(f.read())

            message_crypt = symetric_key.encrypt(data=message_plain)

            with open(room_id_file + "\\encrypted_message", "wb") as f:
                f.write(message_crypt.hex())

        else:
            print("Chybí text k zašifrování!")
            return

    elif operation == "decrypt_aes":
        # open encrypted_message file and decrypt it's content
        # save decrypted content to decrypted_message

        with open(app_dir + "temp/encrypted_message", "rb") as f:
            message_crypt = f.read()

        with open(app_dir + "temp/symetric_key", "rb") as f:
            symetric_key = Fernet(f.read())

        message_plain = symetric_key.decrypt(message_crypt)

        with open(app_dir + "temp/decrypted_message", "wb") as f:
            f.write(base64.urlsafe_b64decode(message_plain))

    
    else:
        print("Neplatná operace! [generate_rsa, generate_aes, decrypt_rsa, encrypt_aes, decrypt_aes]")
        return
    

main()


"""
    elif operation == "generate_aes":
        # not using

        symetric_key = Fernet.generate_key()
        
        with open(app_dir + "temp/symetric_key", "wb") as f:
            f.write(symetric_key)
"""     