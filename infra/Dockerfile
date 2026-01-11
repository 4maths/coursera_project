# =========================
# Stage 1: Build
# =========================
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-system-dev \
    libboost-thread-dev \
    libasio-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# ⚠️ QUAN TRỌNG: copy toàn bộ source (bao gồm web/crow, web/asio, core)
COPY . .

# ⚠️ QUAN TRỌNG: compile với include path KHỚP 100% repo
RUN g++ web/server.cpp \
    -I. \
    -Icore/include \
    -Iweb \
    -Iweb/asio/include \
    -Iweb/crow \
    -O2 -Wall \
    -lpthread -lboost_system -lboost_thread \
    -o server_app


# =========================
# Stage 2: Runtime
# =========================
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libboost-thread1.74.0 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/server_app .
COPY --from=builder /app/web ./web

RUN mkdir -p uploads && chmod 777 uploads

EXPOSE 18080
CMD ["./server_app"]
