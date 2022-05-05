cmake_minimum_required(VERSION 3.18)
project(ws_to_mongo)

set(CMAKE_CXX_STANDARD 17)


include(${CMAKE_SOURCE_DIR}/build/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(ws_to_mongo src/main.cpp src/websocket.h src/msg_handler.h)
target_compile_features(ws_to_mongo PRIVATE cxx_std_17)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(ws_to_mongo ${CONAN_LIBS} PRIVATE Threads::Threads)
