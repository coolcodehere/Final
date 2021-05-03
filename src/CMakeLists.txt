cmake_minimum_required(VERSION 3.19)

project(Final)

LINK_DIRECTORIES( ${CMAKE_BINARY_DIR}/lib )
LINK_DIRECTORIES( ${CMAKE_BINARY_DIR}/bin )

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# For multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

find_package(OpenGL REQUIRED )
if (OpenGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIRS})
    link_libraries(${OpenGL_LIBRARIES})
endif()	

find_package(GLM REQUIRED)
if (GLM_FOUND)
    include_directories(${GLM_INCLUDE_DIRS})
    link_libraries(${GLM_LIBRARIES})
endif()

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

find_package(GLUT REQUIRED)
if (GLUT_FOUND)
	include_directories( ${GLUT_INCLUDE_DIRS} )
	link_libraries(${GLUT_LIBRARY})
endif()

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})
link_libraries(${GLFW_LIBRARIES})

find_package(GLAD REQUIRED)
include_directories(${GLAD_INCLUDE_PATH})
link_libraries(${GLAD_LIBRARIES})

find_package(STBI REQUIRED)
include_directories(${STBI_INCLUDE_DIR})
link_libraries(${STBI_LIBRARY})

set(COMMON_LIBS optimized ${OpenGL_LIBRARIES} ${GLUT_LIBRARY} 
${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${STBI_LIBRARY} ${GLM_LIBRARIES})
set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

SET(APP_SRCS
  source/Final.cpp
)

SET(APP_COMMON
  common/loadShaders.cpp
)

SET(APP_HDRS
	include/mat.h
	include/vec.h
	include/loadShaders.h
	include/camera.h
)

SET(APP_SHADERS
	shader/cube.vert
	shader/cube.frag
	shader/lamp.vert
	shader/lamp.frag
)

SOURCE_GROUP("Common Files" FILES
  ${APP_COMMON}
)

SOURCE_GROUP("Shader Files" FILES
 ${APP_SHADERS} 
)

add_executable(Final  ${APP_SRCS} ${APP_COMMON}  ${APP_HDRS}  ${APP_SHADERS})
target_link_libraries(Final  ${COMMON_LIBS})

include_directories( include )

ADD_CUSTOM_TARGET(debug ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug ${project_binary_dir})
ADD_CUSTOM_TARGET(release ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release ${project_binary_dir})






