include_directories(../../gram/include)

add_executable(gram-math
        main.cpp
        )

target_link_libraries(gram-math /Users/jansvoboda/Dev/build/gram/src/libgram.a)
