# build
FROM debian:bookworm-slim AS build

RUN apt-get update \
    && apt-get install -y --no-install-recommends gcc make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

# Bake the in-image book location into the binary 
RUN make BOOK_PATH=/usr/local/share/mira/performance.bin

# runtime
FROM debian:bookworm-slim

COPY --from=build /src/mira /usr/local/bin/mira
COPY --from=build /src/performance.bin /usr/local/share/mira/performance.bin

# `docker run -it <image>` -> console mode; append uci/xboard to switch.
ENTRYPOINT ["mira"]
