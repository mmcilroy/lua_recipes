cmake_minimum_required( VERSION 2.8 )

project( lua-receipes )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11" )

include_directories( /usr/include/lua5.2 )

link_directories()

add_executable( wrap_object src/wrap_object.cpp )
add_executable( wrap_shared_ptr src/wrap_shared_ptr.cpp )

target_link_libraries( wrap_object lua5.2 )
target_link_libraries( wrap_shared_ptr lua5.2 )
