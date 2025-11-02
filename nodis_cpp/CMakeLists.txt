cmake_minimum_required(VERSION 3.5)
project(nodis_cpp VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

###########
# INSTALL #
###########

install(
  DIRECTORY
    include  
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/
)

########
# TEST #
########

add_executable(nodis_cpp_test test/nodis_cpp_test.cpp)
target_link_libraries(nodis_cpp_test gtest)
add_test(nodis_cpp_test_run nodis_cpp_test)