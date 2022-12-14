from rsa import PublicKey, encrypt, newkeys
from cryptography.fernet import Fernet
from base64 import urlsafe_b64decode, urlsafe_b64encode

import sys

def main():

    # operations -> [generate_rsa, decrypt_rsa, decrypt_aes]
    # arguments -> [key_size: int, b64_text: bytes, b64_text: bytes]

    argv = sys.argv[1:]
    length = len(argv)
    
    if length == 0:
        print("Chybí operace! [generate_rsa, decrypt_rsa, decrypt_aes]")
        return
    
    operation = argv[0].lower()
    
    if operation == "generate_rsa":

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

                public_k, private_k = newkeys(int(argv[1]))

                # save to file
                with open("public_key.pem", "wb") as f:
                    f.write(public_k.save_pkcs1())

                # save to file
                with open("private_key.pem", "wb") as f:
                    f.write(private_k.save_pkcs1())

        else:
            print("Chybí velikost RSA klíče v bitech! [2048, 4096]")
            return
    
    else:
        print("Neplatná operace! [generate_rsa, decrypt_rsa, decrypt_aes]")
        return
    

main()