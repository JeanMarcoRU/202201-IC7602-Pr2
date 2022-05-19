#### Jean Marco Rojas - 2015040717

# Instrucciones de ejecución

## Para el UDP Server

```
gcc -o cserver UDP_server.c -lm
```

```
./cserver
```

### En otra termial:

```
sudo apt-get install ncat
```

```
ncat localhost 5000 -u
```

```
nc -vu localhost 5000
```

## Para el Api REST

Comandos Utiles:

```
sudo pip3 install virtualenv
source venv/bin/activate
deactivate
pip install -r requirements.txt
```


```
sudo docker-compose build
sudo docker-compose up -d
```

Para probar la API:

Para pasar un mensaje a base64:

```
http://127.0.0.1:3000/encode?msj=mensaje a encriptar
```

Para pasar de base64 a texto:

```
http://127.0.0.1:3000/decode?msj=amVhbiBtYXJjbyByb2phcw==
```