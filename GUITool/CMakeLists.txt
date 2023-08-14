cmake_minimum_required(VERSION 3.11)
 
set(CMAKE_CXX_STANDARD 20)

project(GUITool)

find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
  message('OPENGL_FOUND-is-true')
else()
  message('OPENGL_FOUND-is-false')
endif()

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
  ${GUITOOL_DIR}/include/
  ${GUITOOL_DIR}/include/WindowsHandler
  ${GUITOOL_DIR}/include/LabelHandler
  ${GUITOOL_DIR}/include/SimulationHandler
)

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

add_executable(
  GUITool 
  
  ${GUITOOL_DIR}/src/tool.cpp
  ${GUITOOL_DIR}/include/WindowsHandler/Controller.h
  ${GUITOOL_DIR}/include/WindowsHandler/Controller.cpp
  ${GUITOOL_DIR}/include/WindowsHandler/show_control_window.h
  ${GUITOOL_DIR}/include/WindowsHandler/show_control_window.cpp
  ${GUITOOL_DIR}/include/LabelHandler/LabelController.h
  ${GUITOOL_DIR}/include/LabelHandler/LabelController.cpp
  ${GUITOOL_DIR}/include/LabelHandler/show_label_window.h
  ${GUITOOL_DIR}/include/LabelHandler/show_label_window.cpp
  ${GUITOOL_DIR}/include/SimulationHandler/SimulationController.h
  ${GUITOOL_DIR}/include/SimulationHandler/SimulationController.cpp
  ${GUITOOL_DIR}/include/SimulationHandler/show_simulation_window.h
  ${GUITOOL_DIR}/include/SimulationHandler/show_simulation_window.cpp

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

target_link_libraries(
  GUITool
  IMGUI_LIB
  IMPLOT_LIB
  IMFD_LIB
  $<$<BOOL:${WIN32}>:${THIRD_DIR}/windows/lib/glfw3.lib>
  $<$<BOOL:${UNIX}>:glfw>
  ${OPENGL_LIBRARIES}
)

target_compile_features(GUITool PRIVATE cxx_std_20)

if(WIN32)
  set_target_properties(GUITool PROPERTIES WIN32_EXECUTABLE TRUE)
endif()