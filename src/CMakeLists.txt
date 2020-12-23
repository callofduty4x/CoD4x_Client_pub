project(cod4x_client)

# This project make use of custom link logic because of -nostdlib flag.
# So setting up COD4XLINK "language" compiler.
set(CMAKE_COD4XLINK_STANDARD_LIBRARIES ${CMAKE_CXX_STANDARD_LIBRARIES})
set(CMAKE_COD4XLINK_FLAGS ${CMAKE_CXX_FLAGS})
set(CMAKE_COD4XLINK_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS})
set(CMAKE_COD4XLINK_IMPLICIT_LINK_DIRECTORIES ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})

string(APPEND CMAKE_COD4XLINK_CREATE_SHARED_LIBRARY 
    "<CMAKE_COMMAND> -E remove -f <OBJECT_DIR>/objects.a;"
    "<CMAKE_AR> cr <OBJECT_DIR>/objects.a <OBJECTS>;"
    "<CMAKE_CXX_COMPILER> <CMAKE_SHARED_LIBRARY_CXX_FLAGS> <LANGUAGE_COMPILE_FLAGS> "
    "<CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS> -o <TARGET> -Wl,--out-implib,<TARGET_IMPLIB> " 
    "-Wl,--major-image-version,<TARGET_VERSION_MAJOR>,--minor-image-version,<TARGET_VERSION_MINOR> "
    "-Wl,--whole-archive <OBJECT_DIR>/objects.a -Wl,--no-whole-archive <LINK_LIBRARIES> <LINK_FLAGS>"
)

# Static library of client.
file(GLOB SRC *.c *.cpp)
file(GLOB INC *.h *.hpp)
file(GLOB ASM_SRC *.asm)
file(GLOB ASSET_SRC xassets/*.c xassets/*.cpp)
file(GLOB ASSET_INC xassets/*.h xassets/*.hpp)
file(GLOB UI_SRC ui/*.c ui/*.cpp)
file(GLOB UI_INC ui/*.h ui/*.hpp)
file(GLOB ANTIREVERSING_SRC antireversing/*.c antireversing/*.cpp)
file(GLOB ANTIREVERSING_INC antireversing/*.h antireversing/*.hpp)

add_library(${PROJECT_NAME} SHARED 
    ${SRC} ${INC} 
    ${ASSET_SRC} ${ASSET_INC} 
    ${ASM_SRC} 
    ${UI_SRC} ${UI_INC}
    ${ANTIREVERSING_SRC} ${ANTIREVERSING_INC}    
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/freetype-2.9/include)
# Here should be only our own targets. Rest of libraries should be added to language configuration.
target_link_libraries(${PROJECT_NAME} 
    freetype steam_api discord-rpc diskinfo jpg tomcrypt tommath mbedtls udis86 seh
    winmm crypt32 version
)
target_link_options(${PROJECT_NAME} PRIVATE 
    -static -nostdlib ${CMAKE_SOURCE_DIR}/mingwcrt/dllcrt2.o 
    -Wl,-Map=$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.map,-fPic,--stack,8388608
    -lmsvcr100 -lgcc -lstdc++ -lgcc_eh -lmingwex -lmingw32 -lkernel32 -lgdi32
)
# Force disable optimizations.
target_compile_options(${PROJECT_NAME} PRIVATE -O0)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE COD4XDEV NOD3DVALIDATION NOANTIREVERSING)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG FASTCOMPILE)
    target_compile_options(${PROJECT_NAME} PRIVATE -s -O0)
    # Run exeobfus for release build only
    target_link_options(${PROJECT_NAME} PRIVATE -s)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND $<TARGET_FILE:exeobfus> ARGS "$<TARGET_FILE:${PROJECT_NAME}>"
    )
endif()

# Deploy to localappdata
set(ENV_VER $ENV{COD4XCLIENT_CMAKE_BUILD_VERSION})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
   COMMAND ${CMAKE_COMMAND} ARGS -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "$ENV{LOCALAPPDATA}/CallofDuty4MW/bin/cod4x_${ENV_VER}/cod4x_${ENV_VER}.dll"
)

# Override language of ASM_SRC files to ASM_NASM (cmake automatically selects GCC)
# Override whole project language because of custom linker parameters order.
set_source_files_properties(${ASM_SRC} PROPERTIES LANGUAGE ASM_NASM)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE COD4XLINK)
