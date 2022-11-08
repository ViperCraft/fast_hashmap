file(GLOB SRCS *.cpp)

ADD_EXECUTABLE(unittest ${SRCS})

FIND_PACKAGE (Threads)

TARGET_LINK_LIBRARIES(unittest
    libgtest
    libgmock
    ${CMAKE_THREAD_LIBS_INIT}
)

add_test(NAME unittest
         COMMAND unittest)
