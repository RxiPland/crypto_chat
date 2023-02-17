# python 3.11.1

import rsa
from cryptography.fernet import Fernet

import sys
import os
import os.path
import tempfile


app_dir = os.path.dirname(__file__).replace("\\", "/") + "/"
operations = "[generate_id, generate_rsa, encrypt_rsa, decrypt_rsa, generate_aes, encrypt_aes, decrypt_aes]"

def main():

    argv = sys.argv[1:]
    length = len(argv)

    for i in range(length):
        argv[i] = argv[i].strip()
    

    if length == 0:
        raise Exception(f"Operation is missing! {operations}")
    
    operation = argv[0].lower()


    if operation == "generate_rsa":
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

                # generate RSA keys
                public_k, private_k = rsa.newkeys(int(argv[1]))

                # print public key in PEM hex
                print(public_k.save_pkcs1().hex(), end="")
                
                # print ';' for easier splitting
                print(";", end="")

                # print private key in PEM hex
                print(private_k.save_pkcs1().hex(), end="")

        else:
            raise Exception("Size of RSA key is missing! [2048, 4096]")
   

    elif operation == "encrypt_rsa":
        # encrypt plaintext with RSA public key in PEM hex
        # print encrypted text

        if length >= 2:

            if length >= 3:

                public_k = rsa.PublicKey.load_pkcs1(bytes.fromhex(argv[1]))
                plaintext = bytes.fromhex(argv[2])

                encrypted = rsa.encrypt(message=plaintext, pub_key=public_k)

                # print ciphertext in hex
                print(encrypted.hex(), end="")

            else:
                raise Exception("Plaintext in hex is missing!")

        else:
            raise Exception("Public Key in hex is missing!")


    elif operation == "decrypt_rsa":
        # decrypt ciphertext with RSA private key in PEM hex
        # print decrypted text

        if length >= 2:

            if length >= 3:

                private_k = rsa.PrivateKey.load_pkcs1(bytes.fromhex(argv[1]))
                encrypted = bytes.fromhex(argv[2])

                decrypted = rsa.decrypt(crypto=encrypted, priv_key=private_k)

                # print decrypted text in hex
                print(decrypted.hex(), end="")

            else:
                raise Exception("Encrypted text in hex is missing!")

        else:
            raise Exception("Private Key in hex is missing!")
        

    elif operation == "generate_aes":
        # generate symetric key
        # then print it in hex

        key = Fernet.generate_key()

        print(key.hex(), end="")


    elif operation == "encrypt_aes":
        # encrypt hex message with symetric key
        # print OR save to temp file

        """
        add HMAC
        """

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
                    message_crypt_hex = server_symetric_key.encrypt(data=bytes.fromhex(plain_text)).hex()

                    if file_mode:
                        # write encrypted data to temp file
                        with open(temp_file_path, "w") as f:
                            f.write(message_crypt_hex)

                    else:
                        # print encrypted data
                        print(message_crypt_hex, end="")

                else:
                    raise Exception("Plain text in hex OR filename is missing!")
            else:
                raise Exception("\"Saved to file\" boolean is missing!")
        else:
            raise Exception("Room symetric key in hex is missing!")


    elif operation == "decrypt_aes":
        # decrypt hex message with symetric key
        # print OR save to temp file

        """
        add HMAC
        """

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
                        print(message_decrypted_hex, end="")

                else:
                    raise Exception("Encrypted text in hex OR filename is missing!")
            else:
                raise Exception("\"Saved to file\" boolean is missing!")
        else:
            raise Exception("Room symetric key in hex is missing!")

    else:
        raise Exception(f"Invalid operation! {operations}")
    

main()
