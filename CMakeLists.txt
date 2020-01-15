cmake_minimum_required(VERSION 3.12)
project(Texas)

# --------------------------
# Create Texas CMake options
# --------------------------
# START
	option(TEXAS_ENABLE_EVERYTHING "Enables all library functionality." OFF)
	option(TEXAS_ENABLE_KTX_READ "Enables loading KTX files" OFF)
	option(TEXAS_ENABLE_KTX2_READ "Enables loading KTX2 files" OFF)
	option(TEXAS_ENABLE_PNG_READ "Enables loading PNG files" OFF)
	option(TEXAS_ENABLE_VULKAN_TOOLS "Enables tools for interop with Vulkan. Note! Requires Vulkan to be linked!" OFF)
	option(TEXAS_ENABLE_ASSERTIONS "Enables assertions inside the codebase. This can be helpful to catch invalid usage of the interface." OFF)
#
# END
#



# --------------------
# Create CMake library
# --------------------
# START
	set(LIB_NAME ${PROJECT_NAME})
	add_library(${LIB_NAME} INTERFACE)
#
# END
#



# ----------------------
# Link include directory
# ----------------------
# START
	set(INCLUDE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/include")
	target_include_directories(
		${LIB_NAME}
		INTERFACE
		$<BUILD_INTERFACE:${INCLUDE_FOLDER}>
		$<INSTALL_INTERFACE:include>
	)
#
# END
#


# ------------------------
# Link standard .cpp files
# ------------------------
# START

	# Link .cpp files
	set(TEXAS_SRC_FILES 
		"${CMAKE_CURRENT_SOURCE_DIR}/src/Texas.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/MetaData.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/MemReqs.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/Tools.cpp")
	target_sources(${LIB_NAME} INTERFACE ${TEXAS_SRC_FILES})
#
# END
#

# --------------------------
# Handle Texas CMake options
# --------------------------
# START
	if(TEXAS_ENABLE_EVERYTHING)
		set(TEXAS_ENABLE_KTX_READ ON)
		set(TEXAS_ENABLE_KTX2_READ ON)
		set(TEXAS_ENABLE_PNG_READ ON)
		set(TEXAS_ENABLE_ASSERTIONS ON)
	endif()

	# Check that atleast one file format has been enabled
	if(NOT(
		TEXAS_ENABLE_KTX_READ OR
		TEXAS_ENABLE_KTX2_READ OR
		TEXAS_ENABLE_PNG_READ
	))
		message(FATAL_ERROR "FATAL ERROR: To use Texas you must enable atleast one file-format.")
	endif()

	if (TEXAS_ENABLE_KTX_READ)
		target_compile_definitions(${LIB_NAME} INTERFACE TEXAS_ENABLE_KTX_READ)
		target_sources(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/KTX_Read.cpp")
	endif()

	if (TEXAS_ENABLE_PNG_READ)
		target_compile_definitions(${LIB_NAME} INTERFACE TEXAS_ENABLE_PNG_READ)
		target_sources(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/PNG_read.cpp")
		set(TEXAS_LINK_ZLIB 1)
	endif()

	if(TEXAS_ENABLE_VULKAN_TOOLS)
		target_include_directories(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/optional-includes/Vulkan-Tools")
		target_sources(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/VkFormats.cpp")
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
		target_sources(${LIB_NAME} INTERFACE ${TEXAS_ZLIB_SRC_FILES})
	endif()

	if(TEXAS_ENABLE_ASSERTIONS)
		target_compile_definitions(${LIB_NAME} INTERFACE TEXAS_ENABLE_ASSERT)
		target_sources(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/Assert.cpp")
	endif()
#
# END
#


