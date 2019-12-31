cmake_minimum_required(VERSION 3.12)
project(Texas)

# --------------------------
# Create Texas CMake options
# --------------------------
# START

	option(TEXAS_ENABLE_KTX_READ "Enables loading KTX files" OFF)
	option(TEXAS_ENABLE_KTX2_READ "Enables loading KTX2 files" OFF)
	option(TEXAS_ENABLE_PNG_READ "Enables loading PNG files" OFF)

	option(TEXAS_ENABLE_VULKAN_TOOLS "Enables tools for interop with Vulkan" OFF)
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
	set(TEXAS_SRC_FILES 
		"${CMAKE_CURRENT_SOURCE_DIR}/src/Texas.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/MetaData.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/OpenFile.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/OpenBuffer.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/TextureDocument.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/Tools.cpp")
	target_sources(${LIB_NAME} INTERFACE ${TEXAS_SRC_FILES})
#
# END
#

# --------------------------
# Handle Texas CMake options
# --------------------------
# START
	#message(WARNING "Value is: ${TEXAS_ENABLE_KTX_READ}")

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
		target_sources(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/KTX.cpp")
	endif()

	if(TEXAS_ENABLE_VULKAN_TOOLS)
		target_include_directories(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/optional-includes/Vulkan-Tools")
		target_sources(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/VkFormats.cpp")
	endif()

#
# END
#


