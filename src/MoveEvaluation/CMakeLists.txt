set(MOVE_GENERATION_SOURCE_DIR "${PROJECT_SOURCE_DIR}/lib/")
include_directories("${MOVE_GENERATION_SOURCE_DIR}/include")

add_executable(engine main.cpp
    src/eval.cpp
    src/search.cpp
)

target_link_libraries(engine PRIVATE MoveGeneration)
