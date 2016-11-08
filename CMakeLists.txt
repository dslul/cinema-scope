project(cinema CXX)
cmake_minimum_required(VERSION 2.8.10)

# We require at least g++ 4.9, to avoid ABI breakage with earlier versions.
set(cxx_version_required 4.9)
if (CMAKE_COMPILER_IS_GNUCXX)
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${cxx_version_required})
        message(FATAL_ERROR "g++ version must be at least ${cxx_version_required}!")
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

# Search for our dependencies
include(GNUInstallDirs)
find_package(PkgConfig)
find_package(Intltool)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Xml REQUIRED)

pkg_check_modules(
  SCOPE
  libunity-scopes>=0.6.0
  jsoncpp
  net-cpp>=1.1.0
  REQUIRED
)

# Add our dependencies to the include paths
include_directories(
  "${CMAKE_SOURCE_DIR}/src"
  ${SCOPE_INCLUDE_DIRS}
)

# Do not remove these 2 lines, they are required for the correct functionality of the Ubuntu-SDK
set(UBUNTU_MANIFEST_PATH "manifest.json.in" CACHE INTERNAL "Tells QtCreator location and name of the manifest file")
set(UBUNTU_PROJECT_TYPE "Scope" CACHE INTERNAL "Tells QtCreator this is a Scope project")

# Important project paths
set(CMAKE_INSTALL_PREFIX /)
set(SCOPE_INSTALL_DIR "/cinema")
set(GETTEXT_PACKAGE "cinema")
set(PACKAGE_NAME "com.ubuntu.developer.dslul.cinema-scope")
set(SCOPE_NAME "${PACKAGE_NAME}_cinema")

# If we need to refer to the scope's name or package in code, these definitions will help
add_definitions(-DPACKAGE_NAME="${PACKAGE_NAME}")
add_definitions(-DSCOPE_NAME="${SCOPE_NAME}")
add_definitions(-DGETTEXT_PACKAGE="${GETTEXT_PACKAGE}")

# This command figures out the target architecture and puts it into the manifest file
execute_process(
  COMMAND dpkg-architecture -qDEB_HOST_ARCH
  OUTPUT_VARIABLE CLICK_ARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Configure and install the click manifest and apparmor files
configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json DESTINATION "/")
install(FILES "cinema.apparmor" DESTINATION "/")

# Make these files show up in QtCreator
file(GLOB_RECURSE
  _PO_FILES
  "po/*.po"
  "po/*.pot"
)
add_custom_target(hidden_files
  ALL
  SOURCES
    manifest.json.in
    cinema.apparmor
    src/data/cinema.ini.in
    src/data/cinema-settings.ini.in
    po/POTFILES.in
    ${_PO_FILES}
)

# Add our main directories
add_subdirectory(po)
add_subdirectory(src)

# Set up the tests
enable_testing()
add_subdirectory(tests)
add_custom_target(
  check
  ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure
)
