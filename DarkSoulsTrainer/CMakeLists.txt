project(DarkSoulsTrainer)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE INCS "include/*.h")
add_executable(DarkSoulsTrainer ${SOURCES} ${INCS})

source_group(headers FILES ${INCS})

include_directories(${DarkSoulsTrainer_SOURCE_DIR}/include)
target_include_directories(DarkSoulsTrainer SYSTEM PRIVATE "../vendor/asmjit/src")
target_link_libraries(DarkSoulsTrainer PRIVATE asmjit)