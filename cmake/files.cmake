# Add sources to executable/library
target_sources(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/syscall.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/sysmem.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/startup_stm32f407xx.S"
    # Driver sources
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Src/LIS3DSH.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Src/ST7789.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Src/stm32f405xxx_gpio_driver.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Src/stm32f405xxx_i2c_driver.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Src/stm32f405xxx_spi_driver.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Src/stm32f405xxx_adc_driver.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Src/sys_clock.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Src/fft.c"
)

add_library(${PROJECT_NAME}_alt OBJECT
    "${CMAKE_CURRENT_SOURCE_DIR}/Src/main_two.c"
)
set_target_properties(${PROJECT_NAME}_alt PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_include_directories(${PROJECT_NAME}_alt PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/drivers/Inc"
)

target_compile_definitions(${PROJECT_NAME}_alt PRIVATE
    $<$<CONFIG:Debug>:RTE_DEVICE_STARTUP_STM32F4XX;DEBUG>
    $<$<CONFIG:Release>:RTE_DEVICE_STARTUP_STM32F4XX>
)

target_compile_options(${PROJECT_NAME}_alt PRIVATE
    ${CPU_FLAGS}
    ${CC_SECURE}
    $<$<CONFIG:Debug>:
      $<$<COMPILE_LANGUAGE:C>:
        -std=gnu11
        -fstack-usage
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        -O0
        -g3
        -ggdb
      >
    >
    $<$<CONFIG:Release>:
      $<$<COMPILE_LANGUAGE:C>:
        -std=gnu11
        -fstack-usage
        -Wall
        -Wextra
        -Wpedantic
        -Wno-unused-parameter
        -Os
      >
    >
)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/stm32f407xg_flash.ld" "${CMAKE_CURRENT_BINARY_DIR}" COPYONLY)

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/stm32f407xg_flash.ld")
