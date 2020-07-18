LIST(APPEND minelib_libs Neko_Core sdl_engine gles3_wrapper)

file(GLOB_RECURSE minelib_src include/*.h src/*.cpp)

add_library(minelib STATIC ${minelib_src})
target_link_libraries(minelib PUBLIC ${minelib_libs})

target_include_directories(minelib PUBLIC "include/")
neko_lib_config(minelib)
set_target_properties (minelib PROPERTIES FOLDER Neko/Common)

