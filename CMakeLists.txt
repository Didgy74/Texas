cmake_minimum_required(VERSION 3.10)

# -----------------------
# Project-wide configuration
# -----------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Texas)

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

target_include_directories(Texas
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>)


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
	set(TEXAS_LINK_ZLIB ON)
endif()

option(TEXAS_ENABLE_DYNAMIC_ALLOCATIONS "Enables new loading paths that use dynamic allocations." ON)
if(TEXAS_ENABLE_DYNAMIC_ALLOCATIONS)
	target_compile_definitions(Texas
		PUBLIC
			TEXAS_ENABLE_DYNAMIC_ALLOCATIONS)
endif()

if(TEXAS_LINK_ZLIB)
	find_package(ZLIB REQUIRED)
	target_link_libraries(Texas
		PRIVATE
			ZLIB::ZLIB)
endif()


option(TEXAS_BUILD_TESTS "Build test executables." ON)
if (TEXAS_BUILD_TESTS)
	add_executable(compiletest "tests/compiletest.cpp")
	target_link_libraries(compiletest
		PRIVATE
			Texas)
endif()