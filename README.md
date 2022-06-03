#### Jean Marco Rojas - 2015040717, Esteban Cruz - 2018104794, María Madrigal - 2018186563

# Instrucciones de ejecución
Copie y pegue en una terminal levantada en la raíz del proyecto el siguiente comando para construir y ejecutar el proyecto:
```
docker-compose build
docker-compose up
```

Para entrar dentro de un contenedor de ser necesario, usamos:
```
docker exec -it <container> bash
```

Después de esto podrá probar nuestro proyecto haciendo peticiones dns al servidor local por medio de nslookup.
El servidor imprime datos importantes para la comprensión de su funcionamiento, por lo que con el siguiente comando se puede ver lo que le llegó y lo que hizo con eso.
```
docker logs cserver
```
En este proyecto se crean archivos binarios para validar la no pérdida de paquetes, los cuales con la aplicación hexyl se imprimen de manera muy bonita, por lo que se puede ejecutar el siguiente comando para ver el último paquete que arribó al DNS interceptor.
```
hexyl cserver/peticionDNS.bin
```
Para comprobar dicho archivo en binario se puede ejecutar
```
xxd -b cserver/peticionDNS.bin
```
De igual forma para corroborar el paquete DNS de respuesta se puede hacer de la siguiente forma:
```
hexyl cserver/respuestaDNS.bin
xxd -b cserver/respuestaDNS.bin
```

## Para el Api REST

Comandos Útiles:

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
curl -X POST http://127.0.0.1:443/encode/api/dns_resolver?msj=mensaje
curl -X POST http://127.0.0.1:443/decode/api/dns_resolver?msj=bWVuc2FqZQ==
```
## Para Elasticsearch & Kibana

Insertar datos:
```
curl -X POST "http://localhost:9200/zones/_doc/?pretty" -H 'Content-Type: application/json' -d '{"hostname": "www.google.com","TTL": "5","IP": "10.0.5.2"}'
```

Eliminar datos:
```
curl -X DELETE "localhost:9200/zones/_doc/<id>"
```

Obtener datos: 
```
curl -X GET http://0.0.0.0:9200/zones/_doc/_search?q=hostname:www.google.com
```

Modificar datos:
```
curl -X POST "localhost:9200/zones/_doc/<id>/_update?pretty" -H 'Content-Type: application/json' -d '{"doc": { "hostname": "www.google.com", "TTL": "5", "IP": "10.0.0.0"}}'
```
