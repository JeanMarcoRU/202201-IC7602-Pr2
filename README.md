#### Jean Marco Rojas - 2015040717

# Instrucciones de ejecución

```
gcc -o cserver UDP_server.c -lm
```

```
./cserver
```

## En otra termial:

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
curl -H "Content-Type: application/json" -X POST http://localhost:5000/api/todo -d '{"todo": "Learn Mongo"}'
```

```
curl -H "Content-Type: application/json" -X GET http://localhost:5000/api/todo
```
