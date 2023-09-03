cmake_minimum_required(VERSION 3.20)

# -----------------------
# Project 'Texas'
# -----------------------

project(Texas LANGUAGES CXX)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(TEXAS_IS_ROOT_PROJECT TRUE)
else()
	set(TEXAS_IS_ROOT_PROJECT FALSE)
endif()
set(BUILD_SHARED_LIBS FALSE)

include(FetchContent)

# -----------------------
# Target 'Texas'
# -----------------------

add_library(Texas STATIC
	"src/KTX.hpp"
	"src/FileInfo.cpp"
	"src/PNG.hpp"
	"src/PrivateAccessor.hpp"
	"src/Texas.cpp"
	"src/Texture.cpp"
	"src/TextureInfo.cpp"
	"src/Tools.cpp"
	"src/GLTools.cpp"
	"src/VkTools.cpp")

target_compile_features(Texas PUBLIC cxx_std_23)

target_include_directories(Texas PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# -----------------------
# Options for 'Texas'
# -----------------------

option(TEXAS_ENABLE_KTX_READ "Enables loading KTX files" ON)
if (TEXAS_ENABLE_KTX_READ)
	target_compile_definitions(Texas
		PUBLIC
			TEXAS_ENABLE_KTX_READ)
	target_sources(Texas
		PRIVATE
			"src/KTX_Read.cpp")
endif()

option(TEXAS_ENABLE_KTX_SAVE "Enables saving KTX files" ON)
if (TEXAS_ENABLE_KTX_SAVE)
	target_compile_definitions(Texas
		PUBLIC
			TEXAS_ENABLE_KTX_SAVE)
	target_include_directories(Texas
		PUBLIC 
			"optional-includes/KTX_Save")
	target_sources(Texas
		PRIVATE
			"src/KTX_Save.cpp")
endif()

if(NOT(TEXAS_ENABLE_KTX_READ) AND NOT(TEXAS_ENABLE_PNG_READ))
	message(FATAL_ERROR "Texas must be built with support for at least one file format.")
endif()

option(TEXAS_ENABLE_KTX2_READ "Enables saving KTX2 files - NOT READY" ON)
if (TEXAS_ENABLE_KTX2_READ)
	target_compile_definitions(Texas
		PUBLIC
			TEXAS_ENABLE_KTX2_READ)
	target_sources(Texas
		PRIVATE
			"src/KTX2_Read.cpp")
endif()

option(TEXAS_ENABLE_PNG_READ "Enables loading PNG files" ON)
if (TEXAS_ENABLE_PNG_READ)
	target_compile_definitions(Texas
		PUBLIC
			TEXAS_ENABLE_PNG_READ)
	target_sources(Texas
		PRIVATE
			"src/PNG_Read.cpp")
	set(TEXAS_USE_ZLIBNG ON)
endif()

option(TEXAS_ENABLE_DYNAMIC_ALLOCATIONS "Enables new loading paths that use dynamic allocations." ON)
if(TEXAS_ENABLE_DYNAMIC_ALLOCATIONS)
	target_compile_definitions(Texas
		PUBLIC
			TEXAS_ENABLE_DYNAMIC_ALLOCATIONS)
endif()

if(TEXAS_USE_ZLIBNG)
	set(ZLIB_ENABLE_TESTS OFF)
	set(ZLIBNG_ENABLE_TESTS OFF)
	set(WITH_GTEST OFF)
	FetchContent_Declare(
		zlibng
		GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
		GIT_TAG        2.1.3
	)
	FetchContent_MakeAvailable(zlibng)
	if (NOT TARGET zlibstatic)
		message(FATAL_ERROR "zlib-ng target does not exist")
	endif()
	target_link_libraries(Texas PUBLIC zlibstatic)
	# zlibng has issues with FetchContent style, this makes it work.
	target_include_directories(Texas PUBLIC ${zlibng_BINARY_DIR})
endif()


# Set the default value for build-test option
if(TEXAS_IS_ROOT_PROJECT)
	set(TEXAS_BUILD_TESTS_DEFAULT ON)
else()
	set(TEXAS_BUILD_TESTS_DEFAULT OFF)
endif()
option(TEXAS_BUILD_TESTS "Build test executables." ${TEXAS_BUILD_TESTS_DEFAULT})
if (TEXAS_BUILD_TESTS)
	add_executable(compiletest
		"tests/compiletest.cpp")
	target_link_libraries(compiletest
		PRIVATE
			Texas)
endif()