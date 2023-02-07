cmake_minimum_required(VERSION 3.11)
project(MRL_LabelTool)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(WIN32)
  if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    MESSAGE("==================== USING MSVC TO COMILE ====================")
    add_compile_options(/wd4819 /wd4244 /wd4267 /wd4305 "/Zc:__cplusplus")
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/icon/MesIcon.rc")
    set(CMAKE_CXX_FLAGS_DEBUG "/O2")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
  else()
    MESSAGE("==================== USING MINGW TO COMILE ====================")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wa,-mbig-obj") # mingw compile flag (the output was weird idk why).
    set(CMAKE_CXX_FLAGS_DEBUG "-O3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  endif()
else()
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O3")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()
  
set(PROJECT_HEADER ${PROJECT_SOURCE_DIR}/include)
set(MODEL_DIR ${PROJECT_SOURCE_DIR}/Model)
set(THIRD_DIR ${PROJECT_SOURCE_DIR}/3rdparty)
set(EIGEN3_INCLUDE_DIRS ${THIRD_DIR}/Eigen)

include_directories(
  ${EIGEN3_INCLUDE_DIRS}
  ${PROJECT_HEADER}
  ${MODEL_DIR}
  ${MODEL_DIR}/adaboost
  ${MODEL_DIR}/logistic
  ${THIRD_DIR}/imgui
  ${THIRD_DIR}/implot
  ${THIRD_DIR}/ImGuiFileDialog
  ${GUITool}/include/
  ${GUITool}/include/windows_handler
  ${GUITool}/include/label_handler
  ${GUITool}/include/simulation_handler
)

add_executable(Training
  ./src/training.cpp

  ${PROJECT_HEADER}/file_handler.h
  ${PROJECT_HEADER}/file_handler.cpp
  ${PROJECT_HEADER}/make_feature.h
  ${PROJECT_HEADER}/make_feature.cpp
  ${PROJECT_HEADER}/metric.h
  ${PROJECT_HEADER}/metric.cpp

  ${MODEL_DIR}/normalize.h
  ${MODEL_DIR}/normalize.cpp
  ${MODEL_DIR}/adaboost/adaboost.h
  ${MODEL_DIR}/logistic/logistic.h
  ${MODEL_DIR}/logistic/logistic.cpp

  $<$<BOOL:${WIN32}>:${APP_ICON_RESOURCE_WINDOWS}>
)

target_compile_features(Training PRIVATE cxx_std_20)
add_subdirectory(${THIRD_DIR})
add_subdirectory(${PROJECT_SOURCE_DIR}/GUITool)