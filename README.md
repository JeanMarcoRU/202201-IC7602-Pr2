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
hexyl cserver/log.txt
```
Para comprobar dicho archivo en binario, se ejecuta
```
xxd -b cserver/log2.txt
```

## Elasticsearch & Kibana

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

Modificar datos (No es necesario pasar el doc entero, puede ser solo la llave-valor que se quiere modificar):
```
curl -X POST "localhost:9200/zones/_doc/<id>/_update?pretty" -H 'Content-Type: application/json' -d '{"doc": { "hostname": "www.google.com", "TTL": "5", "IP": "10.0.0.0"}}'
```

## Pruebas

Si ponemos en nslookup server 0.0.0.0, podremos usar nuestro dns interceptor pera empezar a crear las pruebas.
![image](https://user-images.githubusercontent.com/15478613/171777588-bb6fa653-6788-4197-8689-b1577c5d73aa.png)

Sientramos a http://0.0.0.0:5601/ ahi podremos ver la base de datos de elasticsearch, con Kibana, donde podemos ver los documentos que hemos creado en el indice "zones".
![image](https://user-images.githubusercontent.com/15478613/171777775-c3dec8b2-482e-47ab-921f-e46e2745e473.png)

En la captura anterior podemos ver que www.google.com tiene 3 ips, por lo que con nslookup podemos consultar esa direccion y nos devolvera un ip diferente haciendo un RoundRobin sobre los mismos.

![image](https://user-images.githubusercontent.com/15478613/171778125-7b54132d-2244-4943-abfd-417158f608e7.png)

Ahora si eliminamos ese documento y volvemos a hacer la peticion, como ya no tenemos www.google.com en nuestra base de datos, la enviara a la api de python mediante http, el cual devolvera el ip real de www.google.com como vemos a continuacion:

![image](https://user-images.githubusercontent.com/15478613/171778536-0e100713-c4b2-443c-8bc3-b9149c73d413.png)

Tambien podemos consultar con el nombre de dominio que queramos, aqui algunos ejemplos:

![image](https://user-images.githubusercontent.com/15478613/171779021-103b7389-931c-4d49-b49c-a951e182dc06.png)

Tambien podemos cambiar el dns externo que usa la api de python para sus consultas, modificando el archivo <./restapi/config.txt>, algunos populares son los de google, <8.8.8.8> o <1.1.1.1>, pero podemos usar cualquier otro.

![image](https://user-images.githubusercontent.com/15478613/171779358-9dade826-dbe8-417c-b9b3-0e2ff7b5e72c.png)

Ahora despues de todas estas pruebas, podemos hacer la prueba final y definitiva, modificar resolv.conf de nuestro sistema operativo Linux:
Primero para modificarlo escribimos en la terminal:
```
sudo nano /etc/resolv.conf
```

Luedo modificamos el archivo, cambiando donde dice nameserver, por nuestro dns, asi:

![image](https://user-images.githubusercontent.com/15478613/171779956-81066bb5-a149-47de-a005-313b81726dce.png)


## Recomendaciones

## Conclusiones

## Referencias Bibliográficas
