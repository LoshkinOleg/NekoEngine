include_directories("include/")

file(GLOB_RECURSE player_test_src include/*.h src/*.cpp)

file(GLOB_RECURSE shaders_src ${CMAKE_SOURCE_DIR}/data/shaders/*.vert ${CMAKE_SOURCE_DIR}/data/shaders/*.frag)

source_group("Shaders"	FILES ${shaders_src})

add_executable(player_test ${player_test_src} ${shaders_src})
target_link_libraries(player_test PUBLIC Neko_Core minelib sdl_engine gles3_wrapper)
neko_bin_config(player_test)

if(EMSCRIPTEN)
    get_target_property(bin_link_flags player_test LINK_FLAGS)
    set_target_properties(player_test PROPERTIES LINK_FLAGS
            "${bin_link_flags} -s USE_SDL=2 -s USE_WEBGL2=1 ")
endif()
set_target_properties(player_test PROPERTIES UNITY_BUILD ON)
if(NOT Emscripten)
target_precompile_headers(player_test PRIVATE "include/player_testing_pch.h")
endif()
set_target_properties (player_test PROPERTIES FOLDER Neko\\Main)