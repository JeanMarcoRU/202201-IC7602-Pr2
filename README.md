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



3. RoundRobin, Kibana
4. Verificación de interpretación y construcción de paquetes DNS
5. nslookup consulta al api
6. Prueba final, modificar resolv.conf

## Recomendaciones

## Conclusiones

## Referencias Bibliográficas
