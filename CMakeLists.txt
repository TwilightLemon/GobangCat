cmake_minimum_required(VERSION 3.2)
project(GobangCat)

set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/Resource)
set(DEST_DIR ${CMAKE_BINARY_DIR})

# 复制资源目录到生成目录
file(COPY ${SOURCE_DIR} DESTINATION ${DEST_DIR})

add_executable(GobangCat
        main.cpp
        InfoBoard.cpp
        InfoBoard.h
        Player/RandomRobot.cpp
        DataHelper/DataType.h
        BoardDrawer.cpp
        BoardDrawer.h
        Player/IPlayer.h
        Player/HumanPlayer.cpp
        Player/ChessTreeRobot.cpp
        DataHelper/DataType.cpp
        DataHelper/ChessTree.cpp
        DataHelper/ChessTree.h
        DataHelper/ModelChecker.cpp
        DataHelper/ModelChecker.h
        DataHelper/CountingEvaluator.cpp
        DataHelper/CountingEvaluator.h)
find_package(raylib)
target_link_libraries(GobangCat raylib)