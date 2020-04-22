cmake_minimum_required(VERSION 3.12)
project(Texas)

# --------------------------
# Create Texas CMake options
# --------------------------
# START
    option(TEXAS_ENABLE_KTX_READ "Enables loading KTX files" ON)
    option(TEXAS_ENABLE_KTX_SAVE "Enables saving KTX files" ON)
    option(TEXAS_ENABLE_PNG_READ "Enables loading PNG files" ON)
    option(TEXAS_ENABLE_DYNAMIC_ALLOCATIONS "Enables new loading paths that use dynamic allocations." ON)
#
# END
#

# --------------------
# Create CMake library
# --------------------
# START
    # Link .cpp files
    set(TEXAS_SRC_FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/src/KTX.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PNG.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/PrivateAccessor.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Texas.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Texture.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/TextureInfo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Tools.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/GLTools.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/VkTools.cpp")
    add_library(Texas STATIC ${TEXAS_SRC_FILES})
#
# END
#

# ----------------------
# Link include directory
# ----------------------
# START
    set(TEXAS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_include_directories(Texas PUBLIC ${TEXAS_INCLUDE_DIR})
    target_include_directories(
        Texas
        PUBLIC
        $<BUILD_INTERFACE:${TEXAS_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    )
#
# END
#

# --------------------------
# Handle Texas CMake options
# --------------------------
# START
    # Check that atleast one file format has been enabled
    if(NOT(
        TEXAS_ENABLE_KTX_READ OR
        TEXAS_ENABLE_PNG_READ
    ))
        message(FATAL_ERROR "FATAL ERROR: To use Texas you must enable atleast one file-format.")
    endif()

    if (TEXAS_ENABLE_KTX_READ)
        target_compile_definitions(Texas PUBLIC TEXAS_ENABLE_KTX_READ)
        target_sources(Texas PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/KTX_Read.cpp")
    endif()

    if (TEXAS_ENABLE_KTX_SAVE)
        target_compile_definitions(Texas PUBLIC TEXAS_ENABLE_KTX_SAVE)
        target_include_directories(Texas PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/optional-includes/KTX_Save")
        target_sources(Texas PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/KTX_Save.cpp")
    endif()

    if (TEXAS_ENABLE_PNG_READ)
        target_compile_definitions(Texas PUBLIC TEXAS_ENABLE_PNG_READ)
        target_sources(Texas PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/PNG_Read.cpp")
        set(TEXAS_LINK_ZLIB 1)
    endif()

    if(TEXAS_ENABLE_DYNAMIC_ALLOCATIONS)
        target_compile_definitions(Texas PUBLIC TEXAS_ENABLE_DYNAMIC_ALLOCATIONS)
    endif()

    if(${TEXAS_LINK_ZLIB})
        set(TEXAS_ZLIB_SRC_FILES 
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/adler32.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/crc32.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/deflate.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/gzclose.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/gzlib.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/gzread.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/gzwrite.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/infback.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/inffast.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/inflate.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/inftrees.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/trees.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/uncompr.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/zlib/zutil.c")
        add_library(zlib STATIC ${TEXAS_ZLIB_SRC_FILES})
        if(MSVC)
            target_compile_definitions(zlib PRIVATE _CRT_SECURE_NO_WARNINGS)
            target_compile_options(zlib PRIVATE /wd4996 /wd4267 /wd6297 /wd26451)
        endif()
        target_link_libraries(Texas PRIVATE zlib)

    endif()
#
# END
#
