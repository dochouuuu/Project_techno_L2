cmake_minimum_required(VERSION 3.3)
project(game C)

include(CTest)
enable_testing()

set(CMAKE_C_FLAGS "-std=c99 -g -Wall")

link_directories(${CMAKE_SOURCE_DIR})

add_library(game game.c game_aux.c)

add_executable(game_text game_text.c)
add_executable(game_test_trdo game_test_trdo.c)
add_executable(game_test_ldrion game_test_ldrion.c)
add_executable(game_test_echaal game_test_echaal.c)


target_link_libraries(game_text game)
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

add_test(test_game_get_piece_shape ./game_test_trdo test_game_get_piece_shape)
add_test(test_game_get_piece_orientation ./game_test_trdo test_game_get_piece_orientation)
add_test(test_game_play_move ./game_test_trdo test_game_play_move)
add_test(test_game_won ./game_test_trdo test_game_won)
add_test(test_game_reset_orientation ./game_test_trdo test_game_reset_orientation)
add_test(test_game_shuffle_orientation ./game_test_trdo test_game_shuffle_orientation)
add_test(test_game_print ./game_test_trdo test_game_print)

add_test(test_game_default ./game_test_ldrion test_game_default) 
add_test(test_game_default_solution ./game_test_ldrion test_game_default_solution)
add_test(test_game_get_adjacent_square ./game_test_ldrion test_game_get_adjacent_square)
add_test(test_game_has_half_edge ./game_test_ldrion test_game_has_half_edge)
add_test(test_game_check_edge ./game_test_ldrion test_game_check_edge)
add_test(test_game_is_connected ./game_test_ldrion test_game_is_connected)
add_test(test_game_is_well_paired ./game_test_ldrion test_game_is_well_paired)