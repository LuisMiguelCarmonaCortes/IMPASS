# CMakeLists.txt del componente principal (src/)

# Buscar todos los archivos .c y .cpp en src/
FILE(GLOB_RECURSE app_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Buscar todos los archivos .c en lib/
FILE(GLOB_RECURSE lib_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib/*.c"
)

# Combinar todas las fuentes
set(all_sources ${app_sources} ${lib_sources})

# Registrar el componente con ESP-IDF
idf_component_register(
    SRCS "${all_sources}"           # Archivos fuente de src/ y lib/
    INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../include" "${CMAKE_CURRENT_SOURCE_DIR}/../lib"  # Cabeceras
)