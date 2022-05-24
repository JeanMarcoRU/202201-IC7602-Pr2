#### Jean Marco Rojas - 2015040717
#### Esteban Cruz - 2018104794
#### María Madrigal - 2018186563

# Instrucciones de ejecución
Copie y pegue en una terminal levantada en la raíz del proyecto los siguientes comandos:
```
docker-compose up -d
docker exec -it cliente bash
```
Después de esto se le abrirá una consola con dnsutils instalado, por lo que podrá ejecutar el comando
```
nslookup www.facebook.com
```
Una vez hecho esto puede ver en los logs del server udp lo que recibe el señor servidor udp en caracteres por medio del siguiente comando:
```
docker logs cserver
```
Para observar los bytes del paquete enviado se ejecuta
```
hexyl log.txt
```
Para comprobar dicho archivo en binario, se ejecuta
```
xxd -b log.txt
```

## Para el Api REST

Comandos Utiles:

```
sudo pip3 install virtualenv
source venv/bin/activate
deactivate
pip install -r requirements.txt
```

Levantar el contenedor en docker:

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

Podemos usar curl
```
curl http://127.0.0.1:3000/decode?msj=SG9sYU11bmRv
```
