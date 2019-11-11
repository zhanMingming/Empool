cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)

project(zmpool)

find_package(GTEST REQUIRED)
find_package(BOOST REQUIRED)
#find_package(PTHREAD REQUIRED)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")

INCLUDE_DIRECTORIES(include)

# execute file
#add_executable(${PROJECT_NAME} ${SOURCES})


#Generate the shared library from the sources
add_library(${PROJECT_NAME} SHARED ${SOURCES})


#Generate the static library from the sources
#add_library(${PROJECT_NAME} STATIC ${SOURCES})


target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${BOOST_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARIES})


INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/lib)
