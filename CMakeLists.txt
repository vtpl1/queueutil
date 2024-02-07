# *****************************************************
#    Copyright 2022 Videonetics Technology Pvt Ltd
# *****************************************************
cmake_minimum_required(VERSION 3.15)

set(VERSION 1.0.9)

project(queueutil
	VERSION ${VERSION}
	DESCRIPTION "A simple wrapper around poco data"
	HOMEPAGE_URL "https://github.com/vtpl1/queueutil"
	LANGUAGES CXX)
set(COMPONENT1 queueutil)

find_package(Catch2 REQUIRED)
find_package(absl REQUIRED)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
set(CMAKE_CXX_CLANG_TIDY
	clang-tidy;)
set(CMAKE_C_CLANG_TIDY
	clang-tidy;)
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

list(APPEND COMPONENT1_PUBLIC_HEADERS
	include/queueutil.h
	include/buffer_queue.h
	include/raw_buffer.h
	include/queue_manager.h
	${PROJECT_BINARY_DIR}/${COMPONENT1}_export.h
	${PROJECT_BINARY_DIR}/version.h)

# set(BUILD_SHARED_LIBS TRUE)
add_library(${COMPONENT1}
    src/queueutil.cpp
	src/raw_buffer.cpp
)

target_compile_features(${COMPONENT1}
	PUBLIC cxx_std_17
)


if (MSVC)
    target_compile_options(${COMPONENT1}
        PUBLIC /Zc:__cplusplus
    )
endif()

target_compile_definitions(${COMPONENT1}
	PUBLIC VTPL_COMPILED_LIB
)

# target_sources(${COMPONENT1} PUBLIC FILE_SET HEADERS
#   FILES ${COMPONENT1_PUBLIC_HEADERS})

add_library(${PROJECT_NAME}::${COMPONENT1} ALIAS ${COMPONENT1})

configure_file(inc/version.h.in version.h)

# These tests can use the Catch2-provided main
add_executable(tests
	test/tests.cpp
)

target_link_libraries(tests
	PRIVATE Catch2::Catch2WithMain
	PRIVATE queueutil
)

include(GenerateExportHeader)
generate_export_header(${COMPONENT1})
include(GNUInstallDirs)
target_include_directories(${COMPONENT1}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
		$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>)

set_target_properties(
	${COMPONENT1} PROPERTIES
		POSITION_INDEPENDENT_CODE ON)

set_target_properties(
	${COMPONENT1} PROPERTIES
		SOVERSION	${PROJECT_VERSION_MAJOR}
		VERSION ${PROJECT_VERSION})

target_link_libraries(${COMPONENT1}
	PUBLIC absl::strings
)

## Install library + targets ##################################################
include(CMakePackageConfigHelpers)

set(INSTALL_TARGETS ${COMPONENT1})

install(TARGETS ${INSTALL_TARGETS}
	EXPORT ${PROJECT_NAME}-targets
	# on Windows put the dlls into bin
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		COMPONENT			${PROJECT_NAME}_Runtime

	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT			${PROJECT_NAME}_Runtime
		NAMELINK_COMPONENT	${PROJECT_NAME}_Development

	# ... and the import lib into the devel package
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT			${PROJECT_NAME}_Development

	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
		COMPONENT			${PROJECT_NAME}_Development
)

# # Use a namespace because CMake provides better diagnostics for namespaced
# # imported targets.
# export(TARGETS ${INSTALL_TARGETS} NAMESPACE ${PROJECT_NAME}::
# 	FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake)

install(EXPORT  ${PROJECT_NAME}-targets
	DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
	NAMESPACE  ${PROJECT_NAME}::)

install(TARGETS ${INSTALL_TARGETS}
	# on Windows put the dlls into bin
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		COMPONENT			${PROJECT_NAME}_Runtime

	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT			${PROJECT_NAME}_Runtime
		NAMELINK_COMPONENT	${PROJECT_NAME}_Development

	# ... and the import lib into the devel package
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		COMPONENT			${PROJECT_NAME}_Development

	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
		COMPONENT			${PROJECT_NAME}_Development
)

configure_package_config_file(
	${PROJECT_SOURCE_DIR}/cmake/config.cmake.in
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
	NO_SET_AND_CHECK_MACRO)

write_basic_package_version_file(
    ${PROJECT_NAME}-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(
    FILES
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})

# install(
# 	DIRECTORY
# 		include/
# 	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
# 	PATTERN "hxx" EXCLUDE)

install(
	FILES
		${COMPONENT1_PUBLIC_HEADERS}
		# ${PROJECT_BINARY_DIR}/version.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
