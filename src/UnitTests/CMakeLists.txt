
set(MOVE_GENERATION_SOURCE_DIR "${PROJECT_SOURCE_DIR}/lib/")
include_directories("${MOVE_GENERATION_SOURCE_DIR}/include")

add_executable(SlidingPiecesTest MoveGenerationTest/sliding_pieces.cpp)
add_executable(PerftTest MoveGenerationTest/perft.cpp)

target_link_libraries(SlidingPiecesTest PRIVATE MoveGeneration)
target_link_libraries(PerftTest PRIVATE MoveGeneration)

set_target_properties(SlidingPiecesTest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/../app/bin/tests"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/../app/bin/tests"
)

set_target_properties(PerftTest
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/../app/bin/tests"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/../app/bin/tests"
)

add_test(NAME movgen.sliding.random COMMAND SlidingPiecesTest)

add_test(NAME movgen.perft.start_pos        COMMAND PerftTest 0 5)
add_test(NAME movgen.perft.Kiwipete_pos     COMMAND PerftTest 1 5)
add_test(NAME movgen.perft.pos_3            COMMAND PerftTest 2 5)
add_test(NAME movgen.perft.pos_4            COMMAND PerftTest 3 5)
add_test(NAME movgen.perft.pos_4_flipped    COMMAND PerftTest 4 5)
add_test(NAME movgen.perft.pos_5            COMMAND PerftTest 5 5)
