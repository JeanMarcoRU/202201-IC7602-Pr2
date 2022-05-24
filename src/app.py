from flask import Flask, request
import base64
#from users import users

app = Flask(__name__)

# Routes


@app.route("/", methods=["GET"])
def ping():
    return "hola mundo rest API"


@app.route("/decode/api/dns_resolver", methods=['POST'])
def decodemsj():
    base64_message = request.args.get("msj")
    if not base64_message:
        return "Necesita agregar: ?msj=UHl0aG9uIGlzIGZ1bg== \nEjemplo: http://127.0.0.1:443/decode/api/dns_resolver?msj=UHl0aG9uIGlzIGZ1bg=="
    else:
        base64_bytes = base64_message.encode('ascii')
        message_bytes = base64.b64decode(base64_bytes)
        message = message_bytes.decode('ascii')  
        return message
    
@app.route("/encode/api/dns_resolver", methods=['POST'])
def encodemsj():
    message = request.args.get("msj")
    if not message:
        return "Necesita agregar: ?msj=UHl0aG9uIGlzIGZ1bg== \nEjemplo: http://127.0.0.1:443/encode/api/dns_resolver?msj=Hola Mundo"
    else:
        message_bytes = message.encode('ascii')
        base64_bytes = base64.b64encode(message_bytes)
        base64_message = base64_bytes.decode('ascii')
        return base64_message


# Start the Server
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=443, debug=True)
