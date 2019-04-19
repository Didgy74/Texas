cmake_minimum_required(VERSION 3.12)
project(DTex)

set(CMAKE_CXX_STANDARD 17)

set(LIB_NAME ${PROJECT_NAME})
add_library(${LIB_NAME} INTERFACE)
add_library(${LIB_NAME}::${LIB_NAME} ALIAS ${LIB_NAME})

set(INCLUDE_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(
    ${LIB_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${INCLUDE_FOLDER}>
    $<INSTALL_INTERFACE:include>
)

# Compile example
set(COMPILE_EXAMPLES 1)
if (${COMPILE_EXAMPLES})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
	
	set(EXAMPLE1_NAME "Example")
	add_executable(${EXAMPLE1_NAME} "examples/example.cpp")
	
	target_link_libraries(${EXAMPLE1_NAME} ${LIB_NAME}::${LIB_NAME})

	# Add custom command to copy example resources directory to build directory
	# Remove example resources directory in build directory if present
	add_custom_command(
		TARGET ${EXAMPLE1_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E remove_directory
		$<TARGET_FILE_DIR:${EXAMPLE1_NAME}>/resources
		)

	# Copy directory from source folder to build folder
	add_custom_command(
		TARGET ${EXAMPLE1_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		${PROJECT_SOURCE_DIR}/examples/resources
		$<TARGET_FILE_DIR:${EXAMPLE1_NAME}>/resources
		)
endif()
