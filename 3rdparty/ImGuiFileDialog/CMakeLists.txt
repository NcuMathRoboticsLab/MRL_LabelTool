file(GLOB IMFD_INCLUDE CONFIGURE_DEPENDS 
  ${IMFD_DIR}/*.h
  ${IMFD_DIR}/*.cpp
)

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD2")
endif()

add_library(
  IMFD_LIB STATIC
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/GLFW/glfw3.h>
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/GLFW/glfw3native.h>
  ${IMGUI_INCLUDE}
  ${IMFD_INCLUDE}
)

target_link_libraries(
  IMFD_LIB
  IMGUI_LIB
  ${OPENGL_LIBRARIES}
  $<$<BOOL:${WIN32}>:${WINDOWS_INCLUDE}/lib/glfw3.Lib>
  $<$<BOOL:${UNIX}>:glfw>
  glad
)

target_include_directories(
  IMFD_LIB PUBLIC
  ${PROJECT_HEADER}
  ${EIGEN3_INCLUDE_DIRS}
  ${THIRD_DIR}/imgui
  ${THIRD_DIR}/ImGuiFileDialog
  ${OPENGL_INCLUDE_DIRS}
  ${WINDOWS_INCLUDE}
  ${IMFD_DIR}
)