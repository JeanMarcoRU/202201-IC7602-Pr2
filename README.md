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

```
pip install -r requirements.txt
```

Del tutorial: https://www.youtube.com/watch?v=wWXILAcvx00&ab_channel=MarluanEspiritusanto

```
curl -H "Content-Type: application/json" -X POST http://localhost:5000/api/todo -d '{"todo": "Learn Mongo"}'
```

```
curl -H "Content-Type: application/json" -X GET http://localhost:5000/api/todo
```
