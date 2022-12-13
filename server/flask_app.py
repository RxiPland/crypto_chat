from flask import Flask, request
from cryptography.fernet import Fernet

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello from Flask!'


@app.route('/crypt/<zprava>', methods=["GET"])
def sifrovat_zpravu(zprava: str):


    key = Fernet.generate_key()
    f = Fernet(key)

    zasifrovano = f.encrypt(zprava.encode())

    desifrovano = f.decrypt(zasifrovano)
    

    return "Původní zpráva: " + zprava + "<br><br>Vygenerovaný symetrický klíč: " + key.decode()  + "<br><br>Zašifrovaná zpráva: " + zasifrovano.decode() + "<br><br>Dešifrovaná zpráva: " + desifrovano.decode()

if __name__ == "__main__":
    app.run(debug=True, port=5000)