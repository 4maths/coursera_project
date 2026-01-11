# Stage 1: Build - Dùng Ubuntu 22.04 để đồng bộ hoàn toàn với Stage 2
FROM ubuntu:22.04 AS builder

# Tránh các thông báo tương tác làm gián đoạn quá trình build trên AWS
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-system-dev \
    libboost-thread-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Biên dịch ứng dụng
# Biên dịch ứng dụng với đường dẫn include chính xác
RUN g++ web/server.cpp \
    -I. \
    -I./core/include \
    -I./web/asio/include \
    -I./web/crow \
    -O2 -Wall \
    -o server_app \
    -lpthread -lboost_system

# Stage 2: Runtime - Siêu gọn nhẹ để đẩy lên ECR cho nhanh
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libboost-thread1.74.0 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Chỉ copy file thực thi và thư mục web cần thiết
COPY --from=builder /app/server_app .
COPY --from=builder /app/web ./web

# Tạo folder uploads và cấp quyền cho AWS
RUN mkdir -p uploads && chmod 777 uploads

EXPOSE 18080
CMD ["./server_app"]