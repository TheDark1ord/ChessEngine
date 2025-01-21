add_library(MoveGeneration STATIC
    src/MoveGeneration.cpp
    src/MagicNumbers.cpp
    src/MovgenTypes.cpp
    src/Bitboard.cpp
    src/Zobrist.cpp
)

set_target_properties(MoveGeneration PROPERTIES PUBLIC_HEADER
    "./include/MagicNumbers.h;./include/MoveGeneration.h;./include/Types.h")

install(TARGETS MoveGeneration
    LIBRARY DESTINATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    PUBLIC_HEADER DESTINATION "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include"
)

file(GLOB LIB_HEADERS include/*.h)
foreach (HeaderFile IN LISTS LIB_HEADERS)
add_custom_command(TARGET MoveGeneration PRE_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
        "${HeaderFile}"
        "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include/"
    COMMENT "Copying ${HeaderFile} to include\n")
endforeach()

set_target_properties(MoveGeneration PROPERTIES LINKER_LANGUAGE CXX)

# Run tests after comleting the buld
#add_custom_command(
#    TARGET SlidingPiecesTest
#    COMMENT "Run tests"
#    POST_BUILD
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#    COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^SlidingPiecesTest$" --output-on-failures
#)
