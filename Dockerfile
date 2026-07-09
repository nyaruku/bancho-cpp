FROM debian:bookworm-slim AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    ninja-build \
    g++ \
    gcc \
    zlib1g-dev \
    git \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

ARG BUILD_TYPE=Release
ARG HOST=127.0.0.1
ARG PORT=13380
ARG THREADS=2
ARG LEGACY_HEADER=-1

RUN cmake -S . -B build -G Ninja \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        -DCMAKE_C_COMPILER=gcc \
        -DCMAKE_CXX_COMPILER=g++ \
        -DBANCHO_HOST=${HOST} \
        -DBANCHO_PORT=${PORT} \
        -DBANCHO_THREADS=${THREADS} \
        -DBANCHO_LEGACY_HEADER=${LEGACY_HEADER} \
    && cmake --build build --parallel

FROM debian:bookworm-slim

WORKDIR /app
COPY --from=builder /src/build/bancho_cpp .

CMD ["./bancho_cpp"]
