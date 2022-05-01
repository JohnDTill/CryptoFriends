cmake_minimum_required(VERSION 3.5)

project(CryptoFriendshipTest01 LANGUAGES CXX)

find_package(OpenSSL REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BASE ..)
set(META ${BASE}/meta)
set(SRC ${BASE}/src)
set(TEST ${BASE}/test)
set(GEN ${SRC}/generated)
include_directories(${SRC})
include_directories(${GEN})

add_executable(CryptoFriendshipTest01
    ${SRC}/base64.h
    ${SRC}/bytearray.h
    ${SRC}/decrypt.h
    ${SRC}/hex.h
    ${SRC}/text_frequency_analysis.h
    set1.cpp
)

configure_file(${TEST}/4.txt . COPYONLY)
configure_file(${TEST}/6.txt . COPYONLY)
configure_file(${TEST}/6_solved.txt . COPYONLY)
configure_file(${TEST}/7.txt . COPYONLY)
configure_file(${TEST}/7_solved.txt . COPYONLY)
configure_file(${TEST}/8.txt . COPYONLY)

target_link_libraries(CryptoFriendshipTest01 OpenSSL::SSL)

add_custom_target(
    codegen ALL
    COMMAND python3 codegen.py
    WORKING_DIRECTORY ${META}
    BYPRODUCTS ${GEN_FILES}
    COMMENT "Performing codegen"
)

add_dependencies(CryptoFriendshipTest01 codegen)
