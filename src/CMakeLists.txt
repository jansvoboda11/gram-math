include_directories(../lib/gram/src/gram/include)
include_directories(../lib/tinyexpr/src/tinyexpr)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(gram-math
        main.cpp
        MathEvaluator.cpp
        MathEvaluator.h
        StreamLogger.cpp
        StreamLogger.h
        )

add_dependencies(gram-math gram tinyexpr)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(gram-math Threads::Threads)

target_link_libraries(gram-math
        ${gram-math_SOURCE_DIR}/lib/gram/src/gram-build/src/libgram.a
        ${gram-math_SOURCE_DIR}/lib/tinyexpr/src/tinyexpr-build/libtinyexpr.a
        )
