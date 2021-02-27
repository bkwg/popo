cmake_minimum_required(VERSION 3.19.2)
project(popo VERSION 0.1)

add_executable(popo main.c disas.c tracer.c tracee.c argparse.c banner.c args.c)

target_link_libraries(popo PRIVATE Zydis)
target_link_libraries(popo PRIVATE asan)
target_compile_options(popo PRIVATE "-std=c17")
target_compile_options(popo PRIVATE "-pedantic")
target_compile_options(popo PRIVATE "-Wall")
target_compile_options(popo PRIVATE "-Werror")
target_compile_options(popo PRIVATE "-fsanitize=address")

install(TARGETS popo DESTINATION bin)
