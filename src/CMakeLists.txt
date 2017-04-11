include_directories(../lib/gram/src/gram/include)

add_executable(gram-math
        MathEvaluator.cpp
        MathEvaluator.h
        main.cpp
        )

add_dependencies(gram-math gram)

target_link_libraries(gram-math ${gram-math_SOURCE_DIR}/lib/gram/src/gram-build/src/libgram.a)
