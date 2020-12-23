project(steam_api)

file(GLOB SRC *.c *.cpp)
file(GLOB INC *.h *.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC} ${INC})
#target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
