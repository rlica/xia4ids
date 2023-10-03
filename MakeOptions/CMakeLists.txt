############################################################################
#                                                                          #
#       Andres Illana Sison   @ UCM (Spain)  andres.illana@ucm.es          #
#       Copyright (c) 2023. All rights reserved.		      	     		       #
#                                                                          #
############################################################################

cmake_minimum_required(VERSION 3.0)


project(Xia4IDS VERSION 3.0.0 LANGUAGES C CXX)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin)
#set(CMAKE_INSTALL_PREFIX $ENV{HOME}/bin)

#set(CMAKE_BUILD_TYPE Debug) #release MinSizeRel
set(CMAKE_BUILD_TYPE MinSizeRel) #release MinSizeRel

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Net Graf3d Graf Gpad Thread Rint)

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/inc)
add_definitions(${ROOT_CXX_FLAGS} )

set(SOURCES ${PROJECT_SOURCE_DIR}/src/xia4ids.cpp ${PROJECT_SOURCE_DIR}/src/XiaDataListModeMask.cpp ${PROJECT_SOURCE_DIR}/src/XiaData.cpp ${PROJECT_SOURCE_DIR}/src/Unpacker.cpp ${PROJECT_SOURCE_DIR}/src/LDFReader.cpp)
add_executable(xia4ids ${SOURCES})
#add_executable(xia4ids ./src/xia4ids.cpp )

target_link_libraries(xia4ids ${ROOT_LIBRARIES} Spectrum)
install(TARGETS xia4ids DESTINATION ${CMAKE_INSTALL_PREFIX})

# OpenMP lib
find_package(OpenMP REQUIRED)
target_link_libraries(xia4ids OpenMP::OpenMP_CXX)

# Enable PIC
set_property(TARGET xia4ids PROPERTY POSITION_INDEPENDENT_CODE ON)

# Threading
find_package (Threads)
target_link_libraries (xia4ids ${CMAKE_THREAD_LIBS_INIT} )

# C++ flags
target_compile_options(xia4ids PRIVATE -Wunused-result)

# C++17
set_property(TARGET xia4ids PROPERTY CXX_STANDARD 17)
set_property(TARGET xia4ids PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET xia4ids PROPERTY POSITION_INDEPENDENT_CODE ON)

