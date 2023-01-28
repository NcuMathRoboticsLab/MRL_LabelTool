file(GLOB IMFD_INCLUDE CONFIGURE_DEPENDS 
  ${IMFD_DIR}/*.h
  ${IMFD_DIR}/*.cpp
)

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD2")

  add_library(
    IMFD_LIB STATIC
    ${IMGUI_INCLUDE}
    ${IMFD_INCLUDE}
  )

  target_link_libraries(
    IMFD_LIB PUBLIC
    IMGUI_LIB
    glfw
    glad
  )

  target_include_directories(
    IMFD_LIB PUBLIC
    ${THIRD_DIR}/imgui
    ${THIRD_DIR}/ImGuiFileDialog
  )
  
elseif(WIN32)
  add_library(
    IMFD_LIB STATIC
    ${WINDOWS_INCLUDE}/GLFW/glfw3.h
    ${WINDOWS_INCLUDE}/GLFW/glfw3native.h
    ${IMGUI_INCLUDE}
    ${IMFD_INCLUDE}
  )

  target_link_libraries(
    IMFD_LIB
    IMGUI_LIB
    ${OPENGL_LIBRARIES}
    ${WINDOWS_INCLUDE}/lib/glfw3.Lib
    glad
  )

  target_include_directories(
    IMFD_LIB PUBLIC
    ${THIRD_DIR}/imgui
    ${THIRD_DIR}/ImGuiFileDialog
    ${OPENGL_INCLUDE_DIRS}
    ${WINDOWS_INCLUDE}
    ${IMFD_INCLUDE}
  )
endif()


