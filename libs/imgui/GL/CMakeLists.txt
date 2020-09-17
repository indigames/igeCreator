add_library(GLEW STATIC glew.c)
target_include_directories(GLEW PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
#target_compile_definitions(GLEW PUBLIC GLEW_STATIC)
 