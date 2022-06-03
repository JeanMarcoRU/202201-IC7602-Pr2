#### Jean Marco Rojas - 2015040717, Esteban Cruz - 2018104794, María Madrigal - 2018186563

# Instrucciones de ejecución
Copie y pegue en una terminal levantada en la raíz del proyecto el siguiente comando para construir y ejecutar el proyecto:
```
docker-compose build
docker-compose up
```

Para entrar dentro de un contenedor, en caso de ser necesario, usamos:
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

Si entramos a http://0.0.0.0:5601/ , podremos ver la base de datos de elasticsearch, con Kibana, donde podemos ver los documentos que hemos creado en el índice "zones".
![image](https://user-images.githubusercontent.com/15478613/171777775-c3dec8b2-482e-47ab-921f-e46e2745e473.png)

En la captura anterior podemos ver que www.google.com tiene 3 ips, por lo que con nslookup podemos consultar esa dirección y nos devolverá un ip diferente haciendo un RoundRobin sobre los mismos.

![image](https://user-images.githubusercontent.com/15478613/171778125-7b54132d-2244-4943-abfd-417158f608e7.png)

Ahora si eliminamos ese documento y volvemos a hacer la petición, como ya no tenemos www.google.com en nuestra base de datos, la enviará a la api de Python mediante http, el cual devolvera el ip real de www.google.com como vemos a continuación:

![image](https://user-images.githubusercontent.com/15478613/171778536-0e100713-c4b2-443c-8bc3-b9149c73d413.png)

También podemos consultar con el nombre de dominio que queramos, aquí algunos ejemplos:

![image](https://user-images.githubusercontent.com/15478613/171779021-103b7389-931c-4d49-b49c-a951e182dc06.png)

También podemos cambiar el dns externo que usa la api de Python para sus consultas, modificando el archivo <./restapi/config.txt>, algunos populares son los de google, <8.8.8.8> o <1.1.1.1>, pero podemos usar cualquier otro.

![image](https://user-images.githubusercontent.com/15478613/171779358-9dade826-dbe8-417c-b9b3-0e2ff7b5e72c.png)

Ahora, después de todas estas pruebas, podemos hacer la prueba final y definitiva, modificar resolv.conf de nuestro sistema operativo Linux:
Primero para modificarlo escribimos en la terminal:
```
sudo nano /etc/resolv.conf
```

Luedo modificamos el archivo, cambiando donde dice nameserver, por nuestro dns, así:

![image](https://user-images.githubusercontent.com/15478613/171779956-81066bb5-a149-47de-a005-313b81726dce.png)

Y una vez hecho eso podemos navegar libremente por internet usando nuestro dns y en la terminal donde levantamos en docker-compose podremos ver como se resuelven todas las peticiones.

![image](https://user-images.githubusercontent.com/15478613/171780259-c68dd941-4708-4a8c-bc69-6c36c027b365.png)


## Recomendaciones
1. Utilizar las funciones correspondientes para manejar tipos de archivo binario, es decir, utilizar "rb", "wb", entre otras.
2. Utilizar variables de tipo unsigned char para no trabajarlos de tipo string.
3. Para resolver el proyecto, es mejor trabajar primero el DNS Interceptor, el DNS API y de último, implementar elasticsearch y Kibana.
4. Es necesario esperar a que Kibana cargue, puede tardar algunos minutos para levantar. Si se intenta utilizar apenas se hace up, la página no cargará o aparecerá un mensaje de que aún no está listo.
5. Para observar los bytes de los archivos es mejor utilizar hexyl, los refleja de una manera comprensible y así no es necesario darles vuelta para encontrar la información necesaria.
6. El paquete que se obtiene al principio, los bytes del mismo y demás procedimientos que se realizan se pueden guardar en archivos para comprobar que no hay pérdida de datos y que las respuestas obtenidas son las correctas.
7. 

## Conclusiones

## Referencias Bibliográficas
