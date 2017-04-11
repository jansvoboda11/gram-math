include_directories(../lib/gram/src/gram/include)
include_directories(../lib/tinyexpr/src/tinyexpr)

add_executable(gram-math
        ../lib/tinyexpr/src/tinyexpr/tinyexpr.c
        ../lib/tinyexpr/src/tinyexpr/tinyexpr.h
        MathEvaluator.cpp
        MathEvaluator.h
        main.cpp
        )

add_dependencies(gram-math gram tinyexpr)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(gram-math Threads::Threads)

target_link_libraries(gram-math ${gram-math_SOURCE_DIR}/lib/gram/src/gram-build/src/libgram.a)
