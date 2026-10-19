cmake_minimum_required(VERSION 3.25...4.4)
project(tonyMesh CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Supporto per macOS e Homebrew (percorsi prefissi per librerie)
# -----------------------------------------------------------------------------
if(APPLE)
    list(PREPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
    if(EXISTS "/opt/homebrew/opt/libomp")
        set(OpenMP_ROOT "/opt/homebrew/opt/libomp")
    elseif(EXISTS "/usr/local/opt/libomp")
        set(OpenMP_ROOT "/usr/local/opt/libomp")
    endif()
endif()

# -----------------------------------------------------------------------------
# 1. OpenMP
# -----------------------------------------------------------------------------
find_package(OpenMP REQUIRED)

# -----------------------------------------------------------------------------
# 2. Eigen3
# Cerca nei repository di pacchetti (Homebrew / sistema).
# Se non presente, scarica l'ultima release stabile dal repository ufficiale GitLab.
# -----------------------------------------------------------------------------
find_package(Eigen3 QUIET)

if(NOT TARGET Eigen3::Eigen)
    include(FetchContent)
    message(STATUS "Eigen3 non trovato nel sistema. Scaricamento dal repository GitLab...")
    FetchContent_Declare(
        Eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Eigen)
endif()

# -----------------------------------------------------------------------------
# 3. Libreria tonyMesh
# Raccoglie tutti i file .cpp in src/ tranne main.cpp
# -----------------------------------------------------------------------------
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(tonyMesh STATIC ${LIB_SOURCES})

# Include directory: PUBLIC permette sia a tonyMesh che all'eseguibile che la linka
# di fare `#include <nome_file.h>` per qualsiasi header presente in include/
target_include_directories(tonyMesh PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dipendenze: PUBLIC propaga automaticamente Eigen e OpenMP
target_link_libraries(tonyMesh PUBLIC
    Eigen3::Eigen
    OpenMP::OpenMP_CXX
)

# -----------------------------------------------------------------------------
# 4. Eseguibile
# Crea il target eseguibile collegando la libreria tonyMesh
# -----------------------------------------------------------------------------
add_executable(tonyMesh_app "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
set_target_properties(tonyMesh_app PROPERTIES OUTPUT_NAME "tonyMesh")

# Linka la libreria tonyMesh (eredita automaticamente include/, Eigen e OpenMP)
target_link_libraries(tonyMesh_app PRIVATE
    tonyMesh
)
