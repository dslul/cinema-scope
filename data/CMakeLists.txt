configure_file(
  "com.ubuntu.developer.dslul.cinema_cinema-settings.ini"
  "${CMAKE_BINARY_DIR}/src/com.ubuntu.developer.dslul.cinema_cinema-settings.ini"
)

# Install the scope ini file
install(
  FILES "com.ubuntu.developer.dslul.cinema_cinema.ini"
  DESTINATION ${SCOPE_INSTALL_DIR}
)

# Install the scope images
install(
  FILES
    "icon.png"
    "logo.png"
    "screenshot.png"
  DESTINATION
    "${SCOPE_INSTALL_DIR}"
)

