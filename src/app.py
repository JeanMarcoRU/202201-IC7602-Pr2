from flask import Flask, request
import base64
import socket

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


@app.route("/api/dns_resolver", methods=['POST'])
def dns_resolver():
    
    # Recibe los bytes en base64
    data = request.args.get("data") #http://127.0.0.1:443/api/dns_resolver?data=aG9sYSBtdW5kbyBYRA==
    #rint(base64.b64decode(data))
    
    with open("log.txt", "wb") as f:
        f.write(base64.b64decode(data))
    
    dns = "8.8.8.8"
    port = 53
    serverAddressPort = (dns, port)
    bufferSize = 2048
    
    # Create a UDP socket at client side
    UDPClientSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    
    # Send to server using created UDP socket
    UDPClientSocket.sendto(base64.b64decode(data), serverAddressPort)
    
    #tener cuidado por que lo se podria estar manejando como texto
    msgFromServer = UDPClientSocket.recvfrom(bufferSize)
    
    #Lo devulve en base 64
    return base64.b64encode(msgFromServer) 


# Start the Server
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=443, debug=True)
