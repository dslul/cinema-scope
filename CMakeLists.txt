project(cinema-scope CXX)
cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

# We require g++ 4.9, to avoid ABI breakage with earlier version.
set(cxx_version_required 4.9)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (NOT CMAKE_CXX_COMPILER_VERSION MATCHES "^${cxx_version_required}")
        message(FATAL_ERROR "g++ version must be ${cxx_version_required}!")
    endif()
endif()

# Set strict and naggy C++ compiler flags, and enable C++11
add_definitions(
  -fno-permissive
  -std=c++11
  -pedantic
  -Wall
  -Wextra
  -fPIC
  -DQT_NO_KEYWORDS
)

include(GNUInstallDirs)
find_package(PkgConfig)

# We depend on Boost for string trimming
find_package(
  Boost
  REQUIRED
)

# Search for our dependencies
pkg_check_modules(
  SCOPE
  libunity-scopes>=0.6.0
  net-cpp>=1.1.0
  REQUIRED
)

find_package(Qt5Core REQUIRED)
include_directories(${Qt5Core_INCLUDE_DIRS})

# Add our dependencies to the include paths
include_directories(
  "${CMAKE_SOURCE_DIR}/include"
  ${Boost_INCLUDE_DIRS}
  ${SCOPE_INCLUDE_DIRS}
)

# Do not remove this line, its required for the correct functionality of the Ubuntu-SDK
set(UBUNTU_MANIFEST_PATH "manifest.json.in" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")
set(UBUNTU_PROJECT_TYPE "Scope" CACHE INTERNAL "Tells QtCreator this is a Scope project")

# Important project paths
set(CMAKE_INSTALL_PREFIX /)
set(SCOPE_INSTALL_DIR "/cinema")
set(GETTEXT_PACKAGE "cinema-scope")

# If we need to refer to the scope's name or package in code, these definitions will help
add_definitions(-DPACKAGE_NAME="com.ubuntu.developer.dslul.cinema-scope")
add_definitions(-DSCOPE_NAME="com.ubuntu.developer.dslul.cinema-scope_cinema")
add_definitions(-DGETTEXT_PACKAGE="${GETTEXT_PACKAGE}")

#This command figures out the target architecture and puts it into the manifest file
execute_process(
  COMMAND dpkg-architecture -qDEB_HOST_ARCH
  OUTPUT_VARIABLE CLICK_ARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)

# Install the click manifest
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json DESTINATION "/")
install(FILES "cinema.apparmor" DESTINATION "/")

# Make this file show up in QtCreator
add_custom_target(hidden_files
  ALL
  SOURCES
    manifest.json.in
    "cinema.apparmor"
)

# Add our main directories
add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(po)

# Set up the tests
enable_testing()
add_subdirectory(tests)
add_custom_target(
  check
  ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure
)

