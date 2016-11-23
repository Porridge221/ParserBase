cmake_minimum_required(VERSION 3.6)
project(Parser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp AstNode.cpp AstNode.h ParserBase.cpp ParserBase.h)
add_executable(Parser ${SOURCE_FILES})