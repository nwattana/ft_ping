FROM ubuntu:jammy
LABEL maintainer='narin.tent.ind@gmail.com'

RUN apt-get update && apt-get install -y \
    curl \ 
    net-tools \
    vim \ 
    fish \
    git \
    man-db

RUN yes | unminimize

RUN apt-get update && apt-get install -y \
    sl
    
COPY run.sh /app/run.sh
RUN chmod +x /app/run.sh
WORKDIR /app


RUN apt-get update && apt-get install -y\
    inetutils-ping=2*

CMD ["./run.sh"]