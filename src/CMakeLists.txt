
# Put the ini files in the build directory next to the scope
# .so file so that the test tools can find them.
intltool_merge_translations(
  "data/cinema.ini.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${SCOPE_NAME}.ini"
  ALL
  UTF8
)
intltool_merge_translations(
  "data/cinema-settings.ini.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${SCOPE_NAME}-settings.ini"
  ALL
  UTF8
)

# Install the scope ini files
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${SCOPE_NAME}.ini"
    "${CMAKE_CURRENT_BINARY_DIR}/${SCOPE_NAME}-settings.ini"
  DESTINATION
    ${SCOPE_INSTALL_DIR}
)

# Put the logo file in the build directory next to the scope
# .ini file so that the test tools can find it.
configure_file(
  "data/logo.png"
  "${CMAKE_CURRENT_BINARY_DIR}/logo.png"
  @ONLY
  COPYONLY
)

# Install the scope images
install(
  FILES
    "data/icon.png"
    "data/logo.png"
    "data/screenshot.png"
  DESTINATION
    ${SCOPE_INSTALL_DIR}
)

# Find all the sources
file(GLOB_RECURSE
  SCOPE_SOURCES
  "*.cpp"
  "*.h"
)

# Build a shared library containing our scope code.
add_library(
  scope SHARED
  ${SCOPE_SOURCES}
)

# Link against the object library and our external library dependencies
target_link_libraries(
  scope
  ${SCOPE_LDFLAGS}
)

qt5_use_modules(
  scope
  Core
  Xml
  Gui
)


# Set the correct library output name to conform to the securiry policy 
set_target_properties(
  scope
  PROPERTIES
    OUTPUT_NAME "${SCOPE_NAME}"
)

# Install the scope shared library
install(
  TARGETS scope
  LIBRARY DESTINATION ${SCOPE_INSTALL_DIR}
)
