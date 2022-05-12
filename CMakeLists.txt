cmake_minimum_required(VERSION 3.18)
project(ws_to_mongo)

set(CMAKE_CXX_STANDARD 17)


include(${CMAKE_SOURCE_DIR}/build/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(ws_to_mongo src/main.cpp src/websocket.h src/msg_handler.h src/logger.h)
target_compile_features(ws_to_mongo PRIVATE cxx_std_17)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(ws_to_mongo pthread ${CONAN_LIBS})