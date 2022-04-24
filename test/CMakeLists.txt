cmake_minimum_required(VERSION 3.5)

project(CryptoFriendshipTest01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BASE ..)
set(SRC ${BASE}/src)
include_directories(${SRC})

add_executable(CryptoFriendshipTest01
    ${SRC}/base64.h
    ${SRC}/bytearray.h
    ${SRC}/hex.h
    set1.cpp
)
