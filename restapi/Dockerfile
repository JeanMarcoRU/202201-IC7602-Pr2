FROM python:3.8.10

WORKDIR /app

COPY requirements.txt requirements.txt

RUN pip3 install -r requirements.txt
RUN apt-get update
RUN apt install -y hexyl
RUN apt install -y xxd

COPY . .

CMD ["python", "app.py"]