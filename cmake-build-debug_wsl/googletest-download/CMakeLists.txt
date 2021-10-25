cmake_minimum_required(VERSION 3.8)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
        GIT_REPOSITORY https://hub.fastgit.org/google/googletest.git
        GIT_TAG master
        SOURCE_DIR "/mnt/d/bustub/cmake-build-debug_wsl/googletest-src"
        BINARY_DIR "/mnt/d/bustub/cmake-build-debug_wsl/googletest-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
        )
