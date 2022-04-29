FROM conanio/gcc9:latest as conan_distro

USER root
RUN mkdir /app
WORKDIR /app

COPY /src /app/src
COPY /conanfile.txt /app/src
COPY /CMakeLists.txt /app/src
RUN mkdir /app/src/build


USER conan
RUN sudo chmod -R 777 /app
RUN sudo chmod -R 777 /app/src
RUN sudo chmod -R 777 /app/src/build

RUN conan profile new default --detect
RUN conan profile update settings.compiler.libcxx=libstdc++11 default
RUN conan install ./src --build=missing --install-folder=./src/build
RUN cd ./src/build
RUN cmake .. -DCMAKE_BUILD_TYPE=RELEASE
RUN cmake --build ..

FROM alpine:latest

COPY --from=conan_distro ./src /src

RUN apk add libc6-compat  && \
    apk add libstdc++ && \
    apk add libgcc && \
    apk add gcompat

WORKDIR /src

RUN chmod +x ./build/bin/ws_to_mongo

CMD [ "./build/bin/ws_to_mongo" ]
