#### Jean Marco Rojas - 2015040717

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

## Para el Api REST

Del tutorial: https://www.youtube.com/watch?v=wWXILAcvx00&ab_channel=MarluanEspiritusanto

```
curl -H "Content-Type: application/json" -X POST http://localhost:5000/api/todo -d '{"todo": "Learn Mongo"}'
```

```
curl -H "Content-Type: application/json" -X GET http://localhost:5000/api/todo
```
