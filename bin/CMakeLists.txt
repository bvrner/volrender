set(INCLUDE_DIR ../include/)

set(HEADERS ${INCLUDE_DIR}window.hpp ${INCLUDE_DIR}shader.hpp
            ${INCLUDE_DIR}buffers.hpp)

add_executable(renderer main.cpp ${HEADERS})
target_include_directories(renderer PRIVATE ${INCLUDE_DIR})

# set(OpenGL_GL_PREFERENCE GLVND)
target_link_libraries(renderer PRIVATE ${CONAN_LIBS})
