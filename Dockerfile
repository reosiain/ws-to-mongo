FROM conanio/gcc9:latest as conan_distro

USER root

COPY /src /app/src
RUN mkdir /app/src/build

WORKDIR /app/src

USER conan

RUN sudo chmod -R 777 ./build && \
    cd build && \
    conan install .. --build=missing && \
    cmake .. && \
    cmake --build .

FROM alpine:latest

COPY --from=conan_distro /app/src /app/src

RUN apk add libc6-compat  && \
    apk add libstdc++ && \
    apk add libgcc && \
    apk add gcompat

WORKDIR /app/src

RUN chmod +x ./build/bin/ws_to_mongo

CMD [ "./build/bin/ws_to_mongo" ]

# ENTRYPOINT ["tail", "-f", "/dev/null"]