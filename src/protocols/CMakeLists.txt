macro(add_lib target source)
    add_library(${target} ${source})
    target_link_libraries(${PROJECT_NAME} ${target})
endmacro(add_lib)

add_lib(protocol protocol.cc)
add_lib(blueteeth blueteeth.cc)
add_lib(cereal cereal.cc)