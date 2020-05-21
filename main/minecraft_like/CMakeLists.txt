include_directories("include/")

file(GLOB_RECURSE minecraft_like_src include/*.h src/*.cpp)

add_executable(minecraft_like ${minecraft_like_src})
target_link_libraries(minecraft_like PUBLIC Neko_Core sdl_engine gles3_wrapper)
neko_bin_config(minecraft_like)

if(EMSCRIPTEN)
    get_target_property(bin_link_flags minecraft_like LINK_FLAGS)
    set_target_properties(minecraft_like PROPERTIES LINK_FLAGS
            "${bin_link_flags} -s USE_SDL=2 -s USE_WEBGL2=1 ")
endif()
set_target_properties (minecraft_like PROPERTIES FOLDER Neko)