cmake_minimum_required(VERSION 2.8)

# Application source
set(APPLICATION_SOURCE

    # SIFAKIS SVD
    sifakis/sifakisSVD.cpp

    grid.cpp
    helper.cpp
    initer.cpp
    mesh.cpp
    particle.cpp
    simpleViewer.cpp
    snowModel.cpp
    constants.cpp
    main.cpp
)

#-------------------------------------------------------------------------------
# Set include directories
#-------------------------------------------------------------------------------
include_directories(
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${CGL_INCLUDE_DIRS}
  ${FREETYPE_INCLUDE_DIRS}
)

#-------------------------------------------------------------------------------
# Set link directories
#-------------------------------------------------------------------------------
link_directories(
  ${GLEW_LIBRARY_DIRS}
  ${GLFW_LIBRARY_DIRS}
  ${CGL_LIBRARY_DIRS}
  ${FREETYPE_LIBRARY_DIRS}
)

#-------------------------------------------------------------------------------
# Add executable
#-------------------------------------------------------------------------------
add_executable(mpm ${APPLICATION_SOURCE})

target_link_libraries( mpm
    CGL ${CGL_LIBRARIES}
    glew ${GLEW_LIBRARIES}
    glfw ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${CMAKE_THREADS_INIT}
)

#-------------------------------------------------------------------------------
# Platform-specific configurations for target
#-------------------------------------------------------------------------------
if(APPLE)
  set_property( TARGET mpm APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
endif(APPLE)

# Put executable in build directory root
set(EXECUTABLE_OUTPUT_PATH ..)

# Install to project root
install(TARGETS mpm DESTINATION ${mpm_SOURCE_DIR})
