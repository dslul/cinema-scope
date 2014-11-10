
# Google Mock unfortunately has to be compiled from source
include(FindGMock)

# We need process-cpp to launch the python test server
pkg_check_modules(
  TEST
  process-cpp
  REQUIRED
)

# Include our test library headers
include_directories(
  ${GTEST_INCLUDE_DIRS}
  ${GMOCK_INCLUDE_DIRS}
  ${TEST_INCLUDE_DIRS}
)

# Where to find the scope ini file and .so
add_definitions(
  -DTEST_SCOPE_DIRECTORY="${CMAKE_BINARY_DIR}/src"
)

file(GLOB_RECURSE
  TEST_FIXTURES
  "server/*" 
)

# Make this file show up in QtCreator
add_custom_target(hidden_test_fixtures ALL
  SOURCES ${TEST_FIXTURES}
)

# Where to find the test server binary
add_definitions(
  -DFAKE_SERVER="${CMAKE_CURRENT_SOURCE_DIR}/server/server.py"
)

# Add the unit tests
add_subdirectory(unit)

