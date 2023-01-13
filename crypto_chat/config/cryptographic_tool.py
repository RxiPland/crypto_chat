# python 3.11.1

import rsa
from cryptography.fernet import Fernet

import sys
import os
import os.path
import tempfile
import uuid


app_dir = os.path.dirname(__file__).replace("\\", "/") + "/"
operations = "[generate_rsa, generate_aes, decrypt_rsa, encrypt_aes_server, decrypt_aes_server, get_messages]"

def main():

    argv = sys.argv[1:]
    length = len(argv)

    for i in range(length):
        argv[i] = argv[i].strip()
    

    if length == 0:
        raise Exception(f"Operation is missing! {operations}")
    
    operation = argv[0].lower()

    
    if operation == "generate_rsa":
        # generate random room_id
        # generate RSA key pairs (public & private)
        # print them separated with ';'


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
                
                # print id in hex
                print(room_id.encode().hex())

                # print ';' for easier splitting
                print(";")

                # generate RSA keys
                public_k, private_k = rsa.newkeys(int(argv[1]))

                # print public key in PEM hex
                print(public_k.save_pkcs1().hex())
                
                # print ';' for easier splitting
                print(";")

                # print private key in PEM hex
                print(private_k.save_pkcs1().hex())

        else:
            raise Exception("Size of RSA key is missing! [2048, 4096]")
   

    elif operation == "decrypt_rsa":
        # decrypt RSA cipher to obtain symetric key (AES)
        # save AES to file

        if length >= 2:

            if length >= 3:

                private_k = rsa.PrivateKey.load_pkcs1(bytes.fromhex(argv[1]))
                aes_crypt = bytes.fromhex(argv[2])

                aes_plain = rsa.decrypt(crypto=aes_crypt, priv_key=private_k)

                # print decrypted server AES key in hex
                print(aes_plain.hex())

            else:
                raise Exception("Encrypted AES key is missing!")

        else:
            raise Exception("Private Key is missing!")


    elif operation == "encrypt_aes_server":
        # encrypt user's input with server's key
        # print or save to temp file

        if length >= 2:

            if length >= 3:

                if length >= 4:

                    server_symetric_key = Fernet(bytes.fromhex(argv[1]))

                    file_mode = "true" in argv[2].lower()

                    if file_mode:
                        temp_file_path = tempfile.gettempdir() + f"\\{argv[3]}"

                        with open(temp_file_path, "r") as f:
                            plain_text = f.read().strip()

                    else:
                        plain_text = argv[3]

                    # encrypt plaintext
                    message_crypt_hex = server_symetric_key.encrypt(data=plain_text.encode()).hex()

                    if file_mode:
                        # write encrypted data to temp file
                        with open(temp_file_path, "w") as f:
                            f.write(message_crypt_hex)

                    else:
                        # print encrypted data
                        print(message_crypt_hex)

                else:
                    raise Exception("Plain text OR filename is missing!")
            else:
                raise Exception("\"Saved to file\" boolean is missing!")
        else:
            raise Exception("Symetric key is missing!")


    elif operation == "decrypt_aes_server":
        # decrypt hex message with server symetric key
        # print OR save to temp file

        if length >= 2:

            if length >= 3:

                if length >= 4:

                    server_symetric_key = Fernet(bytes.fromhex(argv[1]))

                    file_mode = "true" in argv[2].lower()

                    if file_mode:
                        temp_file_path = tempfile.gettempdir() + f"\\{argv[3]}"

                        with open(temp_file_path, "r") as f:
                            encrypted_text_hex = f.read().strip()

                    else:
                        encrypted_text_hex = argv[3]

                    # decrypt
                    message_decrypted_hex = server_symetric_key.decrypt(bytes.fromhex(encrypted_text_hex)).hex()

                    if file_mode:
                        # write decrypted data to temp file
                        with open(temp_file_path, "w") as f:
                            f.write(message_decrypted_hex)

                    else:
                        # print decrypted data
                        print(message_decrypted_hex)

                else:
                    raise Exception("Encrypted text OR filename is missing!")
            else:
                raise Exception("\"Saved to file\" boolean is missing!")
        else:
            raise Exception("Symetric key is missing!")


    elif operation == "encrypt_aes_room":
        # encrypt user's input with room's key and save to encrypted_message file

        if length >= 2:

            if length >= 3:

                room_id = argv[1]
                room_id_folder = tempfile.gettempdir() + f"\\{room_id}"
            
                message_plain = bytes.fromhex(argv[2])

                with open(room_id_folder + "\\symetric_key_room", "rb") as f:
                    symetric_key = Fernet(f.read().strip())

                message_crypt = symetric_key.encrypt(data=message_plain)

                print(message_crypt.hex())

            else:
                raise Exception("Plain text missing!")

        else:
            raise Exception("Room ID missing!")


    elif operation == "decrypt_aes_room":
        # open encrypted_message file and decrypt it's content with room symetric key
        # save decrypted content to decrypted_message file

        """
        add HMAC
        """

        if length >= 2:

            if length >= 3:

                room_id = argv[1]
                room_id_folder = tempfile.gettempdir() + f"\\{room_id}"

                message_crypt = bytes.fromhex(argv[2])

                with open(room_id_folder + "\\symetric_key_room", "rb") as f:
                    symetric_key = Fernet(f.read().strip())

                try:
                    message_plain = symetric_key.decrypt(message_crypt)

                    print(message_plain.hex())

                except:
                    
                    print(b'error'.hex())

            else:
                raise Exception("Encrypted message missing!")
        
        else:
            raise Exception("Room ID missing!")

    else:
        raise Exception(f"Invalid operation! {operations}")
    

main()
