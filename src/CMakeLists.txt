set(INCLUDE_DIR ../include/)

set(LIB_SOURCES window.cpp)
set(HEADERS ${INCLUDE_DIR}window.hpp ${INCLUDE_DIR}shader.hpp)

add_library(vol ${LIB_SOURCES} ${HEADERS})
target_include_directories(vol PRIVATE ${INCLUDE_DIR})
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SFML REQUIRED window)

target_link_libraries(vol PRIVATE OpenGL::GL)
target_link_libraries(vol PRIVATE GLEW::GLEW)
target_link_libraries(vol PRIVATE ${SFML_LIBRARIES})
