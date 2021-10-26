cmake_minimum_required(VERSION 3.8)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
        GIT_REPOSITORY https://gitee.com/LiuYoungYoung/googletest.git
        GIT_TAG master
        SOURCE_DIR "D:/bustub/cmake-build-debug-mingw-1/googletest-src"
        BINARY_DIR "D:/bustub/cmake-build-debug-mingw-1/googletest-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        )
