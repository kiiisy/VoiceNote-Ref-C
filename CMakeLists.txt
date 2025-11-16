cmake_minimum_required(VERSION 3.16)
project(audio_dsp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ライブラリ
add_library(dsp_core
    model/DcCut/dc_cut.cpp
    model/NoiseGate/noise_gate.cpp
)

target_include_directories(dsp_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/model
)

# GoogleTest
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# テスト共通ヘルパ
add_library(test_utils
    tests/common/test_utils.cpp
)
target_include_directories(test_utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/common
)
target_compile_definitions(test_utils
    PUBLIC PROJECT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

# 各テスト
function(add_dsp_test name folder)
    add_executable(${name} tests/${folder}/${name}.cpp)
    target_link_libraries(${name}
        PRIVATE
            dsp_core
            test_utils
            gtest
            gtest_main
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

# ここにテストを列挙していく
add_dsp_test(test_dc_cut DcCut)
add_dsp_test(test_noise_gate NoiseGate)
