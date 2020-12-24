cmake_minimum_required(VERSION 3.19.2)
project(popo VERSION 0.1)

add_executable(popo main.c disas.c debug.c argparse.c)

target_link_libraries(popo PRIVATE Zydis)
target_compile_options(popo PRIVATE "-std=c17")
target_compile_options(popo PRIVATE "-pedantic")
target_compile_options(popo PRIVATE "-Wall")

install(TARGETS popo DESTINATION bin)
