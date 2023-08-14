cmake_minimum_required(VERSION 3.11)
project(MRL_LabelTool)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
  
set(PROJECT_HEADER ${CMAKE_SOURCE_DIR}/include)
set(MODEL_DIR ${CMAKE_SOURCE_DIR}/Model)
set(THIRD_DIR ${CMAKE_SOURCE_DIR}/3rdparty)
set(EIGEN3_INCLUDE_DIRS ${THIRD_DIR}/Eigen)

set(GUITOOL_DIR ${CMAKE_SOURCE_DIR}/GUITool)
set(TRAINING_DIR ${CMAKE_SOURCE_DIR}/Training)

add_subdirectory(${THIRD_DIR})
add_subdirectory(${GUITOOL_DIR})
add_subdirectory(${TRAINING_DIR})