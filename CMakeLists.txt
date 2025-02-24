cmake_minimum_required(VERSION 3.5)

# Nome do projeto
PROJECT(CG_Project)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Diretórios do código-fonte
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(CLASS_DIR ${SRC_DIR}/classes)
set(ENGINE_DIR ${SRC_DIR}/engine)
set(GENERATOR_DIR ${SRC_DIR}/generator)

# Ficheiros fonte
set(GENERATOR_SRC ${GENERATOR_DIR}/generator.cpp)

# Criar executáveis
add_executable(generator ${GENERATOR_SRC} ${CLASS_SRC})

# Encontrar OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "OPENGL not found!")
endif()

# Configuração específica para Windows
if(WIN32)
    message(STATUS "Using Windows configuration")

    # Diretório da GLUT
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

    if (NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(FATAL_ERROR "GLUT not found! Set TOOLKITS_FOLDER correctly.")
    endif()

    include_directories(${TOOLKITS_FOLDER}/glut)
    target_link_libraries(generator ${OPENGL_LIBRARIES} ${TOOLKITS_FOLDER}/glut/glut32.lib)

    if (EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif()


# Configuração para Linux/macOS
else()
    message(STATUS "Using Linux/macOS configuration")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
    
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})
    link_directories(${GLUT_LIBRARY_DIRS})
    add_definitions(${GLUT_DEFINITIONS})

    if(NOT GLUT_FOUND)
        message(FATAL_ERROR "GLUT not found!")
    endif()

    target_link_libraries(generator ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
endif()