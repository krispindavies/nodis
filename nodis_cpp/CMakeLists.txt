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

add_executable(core_pub_sub_test test/core_pub_sub_test.cpp)
target_link_libraries(core_pub_sub_test gtest)
add_test(core_pub_sub_test_run core_pub_sub_test)