include_directories(../../gram/include)

add_executable(gram-symbolic-regression
        main.cpp
        )

target_link_libraries(gram-symbolic-regression /Users/jansvoboda/Dev/build/gram/src/libgram.a)
