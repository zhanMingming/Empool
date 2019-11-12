cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)

project(zmpool)

find_package(GTEST REQUIRED)
find_package(BOOST REQUIRED)
#find_package(PTHREAD REQUIRED)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(/usr/local/include)

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


INSTALL(FILES "include/Util.h" DESTINATION include)