cmake_minimum_required(VERSION 2.6)
project(waytk)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" OFF)
option(BUILD_DOCS "Build documentation" OFF)

if(BUILD_DOCS)
	find_package(Doxygen REQUIRED)
endif(BUILD_DOCS)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

pkg_search_module(CAIRO REQUIRED cairo)
pkg_search_module(LIBRSVG REQUIRED librsvg-2.0)
pkg_search_module(WAYLAND_CLIENT REQUIRED wayland-client)
pkg_search_module(WAYLAND_CURSOR REQUIRED wayland-cursor)
pkg_search_module(XKBCOMMON REQUIRED xkbcommon)
pkg_search_module(YAML REQUIRED yaml-0.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(CMAKE_USE_PTHREADS_INIT)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif(CMAKE_USE_PTHREADS_INIT)

include_directories(include)

add_subdirectory(waytk)
if(BUILD_DOCS)
	add_subdirectory(doc)
endif(BUILD_DOCS)
