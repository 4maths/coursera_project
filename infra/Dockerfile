FROM gcc:13 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    libboost-system-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN g++ web/server.cpp \
    -I. \
    -Icore/include \
    -Iweb/asio/include \
    -Iweb/crow \
    -O2 -Wall \
    -lpthread -lboost_system \
    -o server_app


FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/server_app .
COPY --from=builder /app/web ./web
RUN mkdir -p uploads

EXPOSE 18080
CMD ["./server_app"]
