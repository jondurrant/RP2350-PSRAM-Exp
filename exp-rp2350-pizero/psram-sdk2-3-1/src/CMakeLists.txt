add_executable(${NAME}
    main.cpp
)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(${NAME} 
	pico_stdlib
	pico_sha256
	hardware_psram
)

target_compile_definitions(${NAME} PUBLIC 
	PICO_PSRAM_CS_PIN=47
	PICO_PSRAM_SIZE_BYTES=8388608
)

pico_set_linker_script(${NAME} ${CMAKE_SOURCE_DIR}/rp2350-pizero-sram.ld)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

# enable usb output, disable uart output
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)