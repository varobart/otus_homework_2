add_library(version hello.cpp)

target_compile_features(version PUBLIC cxx_std_17)
target_include_directories(version INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sayversion main.cpp)
target_link_libraries(sayversion version)
