cmake_minimum_required(VERSION 3.11)
 
project(GUITool)

set(CMAKE_CXX_STANDARD 17)

set(GUITool ${PROJECT_SOURCE_DIR})

find_package(OpenGL REQUIRED)

include_directories(
  ${OPENGL_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
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

add_executable(
  GUITool 

  ${GUITool}/include/windows_handler/Controller.h
  ${GUITool}/include/windows_handler/Controller.cpp
  ${GUITool}/include/windows_handler/show_control_window.h
  ${GUITool}/include/windows_handler/show_control_window.cpp
  ${GUITool}/include/label_handler/LabelController.h
  ${GUITool}/include/label_handler/LabelController.cpp
  ${GUITool}/include/label_handler/show_label_window.h
  ${GUITool}/include/label_handler/show_label_window.cpp
  ${GUITool}/include/simulation_handler/SimulationController.h
  ${GUITool}/include/simulation_handler/SimulationController.cpp
  ${GUITool}/include/simulation_handler/show_simulation_window.h
  ${GUITool}/include/simulation_handler/show_simulation_window.cpp

  ${PROJECT_HEADER}/file_handler.h
  ${PROJECT_HEADER}/file_handler.cpp
  ${PROJECT_HEADER}/make_feature.h
  ${PROJECT_HEADER}/make_feature.cpp
  ${PROJECT_HEADER}/metric.h
  ${PROJECT_HEADER}/metric.cpp

  ${GUITool}/src/tool.cpp
  ${MODEL_DIR}/normalize.h
  ${MODEL_DIR}/normalize.cpp
  ${MODEL_DIR}/adaboost/adaboost.h
  ${MODEL_DIR}/logistic/logistic.h
  ${MODEL_DIR}/logistic/logistic.cpp

  $<$<BOOL:${WIN32}>: ${APP_ICON_RESOURCE_WINDOWS}>
)

if(OPENGL_FOUND)
  message('OPENGL_FOUND-is-true')
else()
  message('OPENGL_FOUND-is-false')
endif()

target_link_libraries(
  GUITool
  IMGUI_LIB
  IMPLOT_LIB
  IMFD_LIB
  $<$<BOOL:${WIN32}>: ${THIRD_DIR}/windows/lib/glfw3.lib>
  $<$<BOOL:${UNIX}>: glfw>
  ${OPENGL_LIBRARIES}
)