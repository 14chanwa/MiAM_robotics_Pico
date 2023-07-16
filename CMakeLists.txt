cmake_minimum_required(VERSION 3.12)
include(pico_sdk_import.cmake)

set(CMAKE_CXX_STANDARD 17)

project(nrf C CXX ASM)

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )


# pico-ssd1306
add_library(pico_ssd1306
        pico-ssd1306/ssd1306.cpp
        pico-ssd1306/frameBuffer/FrameBuffer.cpp
        pico-ssd1306/shapeRenderer/ShapeRenderer.cpp)

add_subdirectory(pico-ssd1306/textRenderer)

target_link_libraries(pico_ssd1306
        ssd1306_textRenderer
        hardware_i2c
        pico_stdlib
        )
target_include_directories (pico_ssd1306 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/pico-ssd1306)

# miam_utils_pico
FILE( GLOB LIB_SOURCES 
    miam_utils_pico/src/*.cpp 
    miam_utils_pico/src/trajectory/*.cpp
    miam_utils_pico/src/drivers/*.cpp
 )
FILE( GLOB LIB_HEADERS 
    miam_utils_pico/include/*.h 
    miam_utils_pico/include/trajectory/*.h 
    miam_utils_pico/include/drivers/*.h 
)
ADD_LIBRARY( miam_utils_pico ${LIB_SOURCES} ${LIB_HEADERS} )

target_include_directories(miam_utils_pico PUBLIC
miam_utils_pico/include
/usr/include/eigen3
)

target_link_libraries(miam_utils_pico
pico_stdlib
pico_multicore
hardware_spi
hardware_pwm
)

# robot

add_executable(robot)

target_sources(robot PRIVATE 
    robot/main.cpp)

target_include_directories(robot PUBLIC
miam_utils_pico/include
miam_utils_pico/include/drivers
)

target_link_libraries(robot
pico_stdlib
pico_multicore
hardware_spi
hardware_pwm
miam_utils_pico
)

# enable usb output, disable uart output
pico_enable_stdio_usb(robot 0)
pico_enable_stdio_uart(robot 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(robot)


# remote

add_executable(remote
remote/main.cpp
)
target_include_directories(remote PUBLIC
miam_utils_pico/include
miam_utils_pico/include/drivers
)

target_link_libraries(remote
pico_stdlib
hardware_spi
hardware_adc
miam_utils_pico
)


# robot_l298n

add_executable(robot_l298n)

target_sources(robot_l298n PRIVATE 
robot_l298n/main.cpp)

target_include_directories(robot_l298n PUBLIC
miam_utils_pico/include
miam_utils_pico/include/drivers
pico-ssd1306
)

target_link_libraries(robot_l298n
pico_stdlib
pico_multicore
hardware_spi
hardware_pwm
hardware_i2c
miam_utils_pico
pico_ssd1306
)

# enable usb output, disable uart output
pico_enable_stdio_usb(robot_l298n 0)
pico_enable_stdio_uart(robot_l298n 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(robot_l298n)
