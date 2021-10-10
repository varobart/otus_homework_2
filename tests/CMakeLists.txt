add_executable(version-tests test_version.cpp)
target_link_libraries(version-tests version CONAN_PKG::gtest)

add_test(VersionTest "${CMAKE_BINARY_DIR}/bin/version-tests")
