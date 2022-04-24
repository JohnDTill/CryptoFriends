cmake_minimum_required(VERSION 3.5)

project(CryptoFriendshipTest01 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BASE ..)
set(SRC ${BASE}/src)
set(TEST ${BASE}/test)
include_directories(${SRC})

add_executable(CryptoFriendshipTest01
    ${SRC}/base64.h
    ${SRC}/bytearray.h
    ${SRC}/hex.h
    ${SRC}/text_frequency_analysis.h
    set1.cpp
)

configure_file(${TEST}/4.txt . COPYONLY)
configure_file(${TEST}/6.txt . COPYONLY)
configure_file(${TEST}/7.txt . COPYONLY)
configure_file(${TEST}/8.txt . COPYONLY)

target_link_libraries(CryptoFriendshipTest01 OpenSSL::SSL)
