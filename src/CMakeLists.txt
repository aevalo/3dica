find_package(PkgConfig)
pkg_check_modules(SDL2 REQUIRED sdl2)

find_package(Boost 1.57 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS} ${Boost_LIBRARY_DIRS})

add_definitions(${SDL2_CFLAGS_OTHER})

set(3DICA_SOURCES main.cpp)
if(CMAKE_HOST_WIN32)
  set(3DICA_SOURCES ${3DICA_SOURCES} 3dica.rc)
endif(CMAKE_HOST_WIN32)

add_executable(3dica ${3DICA_SOURCES})
target_link_libraries(3dica ${SDL2_LIBRARIES} ${Boost_LIBRARIES})
