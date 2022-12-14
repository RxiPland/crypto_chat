import rsa
from cryptography.fernet import Fernet
import base64

import sys

def main():

    # operations -> [generate_rsa, decrypt_rsa, encrypt_aes, decrypt_aes]
    # arguments -> [key_size: int, b64_text: bytes, b64_text: bytes]

    argv = sys.argv[1:]
    length = len(argv)
    
    if length == 0:
        print("Chybí operace! [generate_rsa, generate_aes ,decrypt_rsa, encrypt_aes, decrypt_aes]")
        return
    
    operation = argv[0].lower()
    
    if operation == "generate_rsa":
        # generate RSA key pairs & save to files


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
                # generate keys
                public_k, private_k = rsa.newkeys(int(argv[1]))

                # save to file
                with open("public_key.pem", "wb") as f:
                    f.write(public_k.save_pkcs1())

                # save to file
                with open("private_key.pem", "wb") as f:
                    f.write(private_k.save_pkcs1())

        else:
            print("Chybí velikost RSA klíče v bitech! [2048, 4096]")
            return

    elif operation == "generate_aes":

        symetric_key = Fernet.generate_key()
        
        with open("symetric_key", "wb") as f:
            f.write(symetric_key)
        

    elif operation == "decrypt_rsa":
        # decrypt RSA to obtain symetric key & save it to file

        if length == 2:
            
            message_crypt = base64.urlsafe_b64decode(argv[1].encode("utf-8"))

            with open("private_key.pem", "rb") as f:
                private_k = rsa.PrivateKey.load_pkcs1(f.read())

            message_plain = rsa.decrypt(crypto=message_crypt, priv_key=private_k)

            with open("symetric_key", "wb") as f:
                f.write(message_plain)


    elif operation == "encrypt_aes":
        # encrypt user's "base64 text" to encrypted_message file

        if length == 2:
        
            message_plain = argv[1].encode("utf-8")

            with open("symetric_key", "rb") as f:
                symetric_key = Fernet(f.read())

            message_crypt = symetric_key.encrypt(data=message_plain)

            with open("encrypted_message", "wb") as f:
                f.write(message_crypt)

        else:
            print("Chybí base64 text!")
            return

    elif operation == "decrypt_aes":
        # open encrypted_message file & decrypt to decrypted_message file

        with open("encrypted_message", "rb") as f:
            message_crypt = f.read()

        with open("symetric_key", "rb") as f:
            symetric_key = Fernet(f.read())

        message_plain = symetric_key.decrypt(message_crypt)

        with open("decrypted_message", "wb") as f:
            f.write(base64.urlsafe_b64decode(message_plain))

    
    else:
        print("Neplatná operace! [generate_rsa, generate_aes, decrypt_rsa, encrypt_aes, decrypt_aes]")
        return
    

main()