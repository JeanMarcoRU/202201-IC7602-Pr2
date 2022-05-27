from flask import Flask, redirect, url_for, request
import base64
import socket
import os

app = Flask(__name__)

# Routes


@app.route("/", methods=["GET"])
def ping():
    return "hola mundo rest API"

@app.route("/api/dns_resolver", methods = ['POST', 'GET'])
def dns_resolver():

    # Recibe los bytes en base64
    # http://127.0.0.1:443/api/dns_resolver?data=aG9sYSBtdW5kbyBYRA==
    data = request.args.get("data")
    # rint(base64.b64decode(data))
    
    decodedata =  base64.b64decode(data)

    with open("log.txt", "wb") as f:
        f.write(decodedata)
        
    os.system("hexyl log.txt")


    serverAddressPort = ("8.8.8.8", 53)  
    bufferSize = 2048

    # Create a UDP socket at client side
    sock = socket.socket(socket.AF_INET,
        socket.SOCK_DGRAM) 

    # Send to server using created UDP socket
    sock.sendto(decodedata, serverAddressPort) #Send message to UDP port

    # tener cuidado por que lo se podria estar manejando como texto
    msgFromServer = sock.recvfrom(bufferSize)
    with open("log2.txt", "wb") as f:
        f.write(msgFromServer[0])
    os.system("hexyl log2.txt")
    
    # Lo devulve en base 64
    return base64.b64encode(msgFromServer[0]).decode('ascii')


# Start the Server
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=443, debug=True)
