include_directories(../lib/gram/src/gram/include)

add_executable(gram-math
        main.cpp
        )

add_dependencies(gram-math gram)

target_link_libraries(gram-math libgram.a)
