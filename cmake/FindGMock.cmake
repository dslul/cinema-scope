# Build with system gmock and embedded gtest
set (GMOCK_INCLUDE_DIRS "/usr/include/gmock/include" CACHE PATH "gmock source include directory")
set (GMOCK_SOURCE_DIR "/usr/src/gmock" CACHE PATH "gmock source directory")
set (GTEST_INCLUDE_DIRS "${GMOCK_SOURCE_DIR}/gtest/include" CACHE PATH "gtest source include directory")

add_subdirectory(${GMOCK_SOURCE_DIR} "${CMAKE_CURRENT_BINARY_DIR}/gmock")

set(GTEST_LIBRARIES gtest)
set(GTEST_MAIN_LIBRARIES gtest_main)
set(GMOCK_LIBRARIES gmock gmock_main)

set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

