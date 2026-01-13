cmake_minimum_required(VERSION 3.3)
project(game C)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "-std=c99 -g -Wall")

link_directories(${CMAKE_SOURCE_DIR})

add_library(game queue.c game.c game_aux.c game_ext.c game_tools.c add_edge.c)
configure_file(${CMAKE_SOURCE_DIR}/game11.txt ${CMAKE_BINARY_DIR}/game11.txt COPYONLY)

## find SDL2
include(sdl2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

## compilation rules
include_directories(${SDL2_ALL_INC})
add_executable(game_sdl game_sdl.c model.c)
target_link_libraries(game_sdl ${SDL2_ALL_LIBS} m game)

## copy useful ressources in the build directory
file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(game_text game_text.c)
add_executable(game_random game_random.c)
add_executable(game_solve game_solve.c)
add_executable(game_test_trdo game_test_trdo.c)
add_executable(game_test_ldrion game_test_ldrion.c)
add_executable(game_test_echaal game_test_echaal.c)

target_link_libraries(game_text game)
target_link_libraries(game_random game)
target_link_libraries(game_solve game)
target_link_libraries(game_test_echaal game)
target_link_libraries(game_test_trdo game)
target_link_libraries(game_test_ldrion game)


add_test(test_trdo_dummy ./game_test_trdo dummy)
add_test(test_ldrion_dummy ./game_test_ldrion dummy)
add_test(test_echaal_dummy ./game_test_echaal dummy)

add_test(test_game_new_empty ./game_test_echaal test_game_new_empty)
add_test(test_game_new ./game_test_echaal test_game_new)
add_test(test_game_copy ./game_test_echaal test_game_copy)
add_test(test_game_equal ./game_test_echaal test_game_equal)
add_test(test_game_delete ./game_test_echaal test_game_delete)
add_test(test_game_set_piece_shape ./game_test_echaal test_game_set_piece_shape)
add_test(test_game_set_piece_orientation ./game_test_echaal test_game_set_piece_orientation )
add_test(test_game_new_empty_ext ./game_test_echaal test_game_new_empty_ext)
add_test(test_game_new_ext ./game_test_echaal test_game_new_ext)

add_test(test_game_get_piece_shape ./game_test_trdo test_game_get_piece_shape)
add_test(test_game_get_piece_orientation ./game_test_trdo test_game_get_piece_orientation)
add_test(test_game_play_move ./game_test_trdo test_game_play_move)
add_test(test_game_won ./game_test_trdo test_game_won)
add_test(test_game_reset_orientation ./game_test_trdo test_game_reset_orientation)
add_test(test_game_shuffle_orientation ./game_test_trdo test_game_shuffle_orientation)
add_test(test_game_print ./game_test_trdo test_game_print)
add_test(test_game_nb_rows_and_nb_cols ./game_test_trdo test_game_nb_rows_and_nb_cols)
add_test(test_game_is_wrapping ./game_test_trdo test_game_is_wrapping)
add_test(test_game_save_load ./game_test_trdo test_game_save_load)

add_test(test_game_default ./game_test_ldrion test_game_default) 
add_test(test_game_default_solution ./game_test_ldrion test_game_default_solution)
add_test(test_game_get_adjacent_square ./game_test_ldrion test_game_get_adjacent_square)
add_test(test_game_has_half_edge ./game_test_ldrion test_game_has_half_edge)
add_test(test_game_check_edge ./game_test_ldrion test_game_check_edge)
add_test(test_game_is_well_paired ./game_test_ldrion test_game_is_well_paired)
add_test(test_game_is_connected ./game_test_ldrion test_game_is_connected)
add_test(test_game_undo ./game_test_ldrion test_game_undo)
add_test(test_game_redo ./game_test_ldrion test_game_redo)
add_test(test_game_solve_nb_solutions ./game_test_ldrion test_game_solve_nb_solutions)
add_test(test_game_solve ./game_test_ldrion test_game_solve)