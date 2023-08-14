cmake_minimum_required(VERSION 3.11)
project(Training)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

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
  ${PROJECT_SOURCE_DIR}/training.cpp

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