project(udis86)

file(GLOB SRC *.c *.cpp)
file(GLOB INC *.h *.hpp)

add_library(${PROJECT_NAME} STATIC ${SRC} ${INC})
