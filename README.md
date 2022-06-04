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

## Elasticsearch & Kibana

### Comandos Útiles:
Insertar datos:
```
curl -X PUT "http://localhost:9200/zones/_doc/<hostname>/?pretty" -H 'Content-Type: application/json' -d '{"hostname": "www.detele.com","TTL": "5","IP": "10.0.5.2"}'

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

Tambien hicimos esta otra prueba con tres terminales al mismo tiempo para probrar la concurrencia del RR en elasticsearch:
![image](https://user-images.githubusercontent.com/15478613/171974492-76ffd58c-d923-4f91-9967-b97c3944d287.png)


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

Luego modificamos el archivo, cambiando donde dice nameserver, por nuestro dns, así:

![image](https://user-images.githubusercontent.com/15478613/171779956-81066bb5-a149-47de-a005-313b81726dce.png)

Y una vez hecho eso podemos navegar libremente por internet usando nuestro dns y en la terminal donde levantamos en docker-compose podremos ver como se resuelven todas las peticiones.

![image](https://user-images.githubusercontent.com/15478613/171780259-c68dd941-4708-4a8c-bc69-6c36c027b365.png)


## Recomendaciones
1. Utilizar las funciones correspondientes para manejar tipos de archivo binario, es decir, utilizar "rb", "wb", entre otras.
2. Utilizar variables de tipo unsigned char para no trabajarlos de tipo string.
3. Para resolver el proyecto, es mejor trabajar primero el DNS Interceptor, el DNS API y de último, implementar elasticsearch y Kibana.
4. Es necesario esperar a que Kibana cargue, puede tardar algunos minutos para levantar. Si se intenta utilizar apenas se hace up, la página no cargará o aparecerá un mensaje de que aún no está listo.
5. Hexyl es una aplicación muy buena para leer archivos binarios para poder interpretarlos. Resultó muy útil para visualizar los patrones presentes en los paquetes DNS y entonces modificarlos o extraer la información pertinente.
6. El paquete que se obtiene al principio, los bytes del mismo y demás procedimientos que se realizan se pueden guardar en archivos para comprobar que no hay pérdida de datos y que las respuestas obtenidas son las correctas.
7. Pasar por volumen la carpeta del api para poder modificar el resolv.conf en el momento de ejecución.
8. En caso de que el puerto 53 no esté disponible, hay que ejecutar
```
sudo systemctl disable systemd-resolved
sudo systemctl stop systemd-resolved
sudo nano /etc/NetworkManager/NetworkManager.conf
```
En [main] se añade 
```
dns=default
```
Luego se ejecuta
```
rm /etc/resolv.conf
sudo systemctl restart NetworkManager
```
Y ya estaría listo.

9. Investigar sobre la biblioteca de curl en C para hacer un GET y POST, la diferencia entre estos, y guardar la respuesta en un archivo (consideramos que es más sencillo para leer después).
10. Investigar cómo armar paquetes de RFC2929.
11. Recomendamos leer cómo hacer las consultas REST a la api de elasticsearch (insertar, eliminar, obtener y modificar datos), utilizando curl desde la terminal en lugar de Kibana.

12. No utilizar usuario y contraseña al utilizar kibana y elasticsearch ya que se podrían presentar muchos errores.

## Conclusiones
1. Se logró implementar completamente el proyecto que consiste en un resolvedor de paquetes DNS con sobre-escritura y round-robin de IPs.
2. Se implementó un servidor UDP en C que recibe, interpreta, traduce a BASE64, redirige y resuelve solicitudes DNS, además con la capacidad de atender a múltiples solicitudes de manera simultánea.
3. Se implementó correctamente una API REST (servidor TCP) para la resolución de peticiones DNS codificadas por medio de BASE64.
4. Se implementó correctamente una base de datos elasticsearch para la sobre-escritura de IPs para los nombres de dominio.
5. Se implementó correctamente un sistema para la utilización de todos los IPs guardados en la base para un dominio particular por medio del método conocido como round-robin.
6. Se logró interpretar, leer, modificar y actualizar paquetes DNS.
7. Se interactuó con especificación formal de redes.
8. Se logró automatizar la ejecución de los componentes de nuestra solución por medio de Docker-Compose.
9. Aprendimos mucho de programar en C.
10. Aprendimos mucho de DNS.
11. Aprendimos a instalar y configurar elasticsearch y kibana, asi como configurarlas sin un usuario y todas las consultas mediantes curl.
12. Aprendimos a levantar una sencilla api rest en python con flask.
## Referencias Bibliográficas
Apellido, N (fecha). *Titulo de la referencia* Recuperado el día X, de https://link \
Open Computers Systems Fundamentals. (2020, 1 enero). *4.6. UDP Socket Programming: DNS* Recuperado 3 de junio de 2022, de https://w3.cs.jmu.edu/kirkpams/OpenCSF/Books/csf/html/UDPSockets.html
* Using Libcurl in C/C++ Application . (2020). Retrieved 1 June 2022, from https://dev.to/hi_artem/using-libcurl-in-c-c-application-4668
