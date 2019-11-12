cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)

project(empool)

#find_package(GTEST REQUIRED)
#find_package(BOOST REQUIRED)
#find_package(THREAD REQUIRED)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")

if(CMAKE_COMPILER_IS_GNUCXX)
    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(CMAKE_CXX_FLAGS " -Wall -pthread -std=c++11 ${CMAKE_CXX_FLAGS}")
        message(STATUS "optional:-std=c++11")   
    else()
        set(CMAKE_CXX_FLAGS "-Wall  -pthread -std=c++11 ${CMAKE_CXX_FLAGS}")
        message(STATUS "optional:-std=c++11")
    endif()
endif(CMAKE_COMPILER_IS_GNUCXX)


INCLUDE_DIRECTORIES(include/empool)
find_path(BOOST_INCLUDE_PATH NAMES  boost/bind.hpp)
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_PATH})
find_package(BOOST REQUIRED)

#INCLUDE_DIRECTORIES(/usr/local/include)

#LINK_DIRECTORIES(/usr/local/include)

# execute file
#add_executable(${PROJECT_NAME} ${SOURCES})


#Generate the shared library from the sources
add_library(${PROJECT_NAME} SHARED ${SOURCES})


#Generate the static library from the sources
#add_library(${PROJECT_NAME} STATIC ${SOURCES})


target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${BOOST_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARIES})


#default CMAKE_INSTALL_PREFIX=/usr/local, so copy targets to this path=/usr/local/lib PATH
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION lib)


#INSTALL(FILES "include/empool/Util.h" DESTINATION include)

INSTALL(DIRECTORY "include/empool" DESTINATION include)