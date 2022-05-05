FROM conanio/gcc9:latest as conan_distro

USER root
RUN mkdir /app
WORKDIR /app

COPY /src /app/src
COPY /conanfile.txt /app
COPY /CMakeLists.txt /app
RUN mkdir /app/build


USER conan
RUN sudo chmod -R 777 /app
RUN sudo chmod -R 777 /app/src
RUN sudo chmod -R 777 /app/build

RUN conan profile new default --detect
RUN conan profile update settings.compiler.libcxx=libstdc++11 default
RUN conan install /app --build=missing --install-folder=/app/build
RUN cmake /app -DCMAKE_BUILD_TYPE=RELEASE && cmake --build /app

FROM alpine:latest

COPY --from=conan_distro /app /app

RUN apk add libc6-compat  && \
    apk add libstdc++ && \
    apk add libgcc && \
    apk add gcompat
    
RUN chmod +x /app/bin/ws_to_mongo

CMD [ "app/bin/ws_to_mongo" ]
