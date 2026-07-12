cmake_minimum_required(VERSION 3.31)

set(NAME test)
set(NAMESPACE kaycxx)
set(PACKAGE_NAME ${NAMESPACE}-${NAME})

project(${PACKAGE_NAME}
    VERSION 0.0.4
    DESCRIPTION "C++ unit test framework"
    HOMEPAGE_URL "https://github.com/${NAMESPACE}/${NAME}"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(cmake/CompilerWarnings.cmake)
include(cmake/GitDeps.cmake)

git_require(kaycxx::assert 0.0.4)
git_require(kaycxx::cli 0.0.6)
git_require(kaycxx::term 0.0.3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)
list(SORT SOURCES)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    include/*.hpp
)
list(SORT HEADERS)

add_library(${PACKAGE_NAME} STATIC)
add_library(${NAMESPACE}::${NAME} ALIAS ${PACKAGE_NAME})

target_sources(${PACKAGE_NAME} PRIVATE
    ${SOURCES}
)

target_sources(${PACKAGE_NAME} PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES ${HEADERS}
)

set_target_properties(${PACKAGE_NAME} PROPERTIES
    OUTPUT_NAME ${PACKAGE_NAME}
    EXPORT_NAME ${NAME}
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
)

target_include_directories(${PACKAGE_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(${PACKAGE_NAME} PUBLIC cxx_std_${CMAKE_CXX_STANDARD})
target_compile_definitions(${PACKAGE_NAME} INTERFACE KAYCXX_ASSERT_SOURCE_LOCATION)
target_link_libraries(${PACKAGE_NAME} PUBLIC kaycxx::assert)
target_link_libraries(${PACKAGE_NAME} PUBLIC kaycxx::cli)
target_link_libraries(${PACKAGE_NAME} PUBLIC kaycxx::term)

include(cmake/Install.cmake)

if(PROJECT_IS_TOP_LEVEL)
    include(cmake/Doxygen.cmake)
    include(cmake/Sandbox.cmake)
    include(cmake/Test.cmake)
endif()
