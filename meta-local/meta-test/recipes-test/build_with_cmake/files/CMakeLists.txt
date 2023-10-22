cmake_minimum_required(VERSION 1.9)
project (hello_with_cmake)
add_executable(hello_with_cmake main.c)
install(TARGETS hello_with_cmake RUNTIME DESTINATION bin)
