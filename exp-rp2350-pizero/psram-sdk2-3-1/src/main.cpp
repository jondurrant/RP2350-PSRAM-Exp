/**
 * Jon Durrant.
 *
 * Blink STATUS LED
 */

#include "pico/stdlib.h"
#include <cstdlib>
#include <cstdio>
#include "pico/sha256.h"
#include "hardware/psram.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"

#define SYS_CLOCK_KHZ 266000 // 266MHz sys clock so PSRAM divisor lands exactly on its 133MHz rated max

#define DELAY 500 // in microseconds
#define TEST_SIZE (1024*32)


int test_ram[TEST_SIZE];

void runTest(int *p, size_t len){
	uint64_t start = to_us_since_boot (get_absolute_time());
	for (int i=0; i < len; i++){
		p[i] = i;
		if (p[i] != i){
			printf(" Write failed\n");
			return;
		}
		if (p[0] != 0){
			printf(" Write failed\n");
			return;
		}
	}

	int total =0;
	for (int i=0; i < len; i++){
		total = p[i] ;
	}
	uint64_t end = to_us_since_boot (get_absolute_time());
	uint64_t ms = end - start;
	printf("Test Completed in %llu us\n", ms);
}

void testSHA(int *p, size_t len){
	pico_sha256_state_t state;
	uint64_t start = to_us_since_boot (get_absolute_time());
	int rc = pico_sha256_start_blocking(&state, SHA256_BIG_ENDIAN, true); // using some DMA system resources
	hard_assert(rc == PICO_OK);
	pico_sha256_update_blocking(&state, (const uint8_t*)p, sizeof(int) * len);

	// Get the result of the sha256 calculation
	sha256_result_t result;
	pico_sha256_finish(&state, &result);

	uint64_t end = to_us_since_boot (get_absolute_time());
	uint64_t ms = end - start;
	printf("SHA Completed in %llu us\n", ms);

	// print resulting sha256 result
	printf("Result:\n");
	for(int i = 0; i < SHA256_RESULT_BYTES; i++) {
		printf("%02x ", result.bytes[i]);
		if ((i+1) % 16 == 0) printf("\n");
	}
}

int main() {
		vreg_set_voltage(VREG_VOLTAGE_1_15);
		sleep_ms(10);
		set_sys_clock_khz(SYS_CLOCK_KHZ, true);

		stdio_init_all();

		gpio_set_function(PICO_PSRAM_CS_PIN, GPIO_FUNC_XIP_CS1); // CS for PSRAM, hardware_psram doesn't set this for us in static (non-auto-detect) size mode

		if (PICO_OK == psram_configure_params(PICO_DEFAULT_PSRAM_MAX_FREQ, PICO_DEFAULT_PSRAM_MAX_SELECT, PICO_DEFAULT_PSRAM_MIN_DESELECT)){
			if (PICO_OK == psram_reinitialize()){
				printf("Initialised PSRAM for APS6404L\n");
			}
		}

		sleep_ms(2000);
		printf("Start\n");

		if (psram_is_available()){
			printf("PSRAM is available of size %lu\n", psram_get_size ());
		}

		// declares and allocates test_psram; falls back to malloc if PSRAM is unavailable/too small
		psram_or_malloc("test_psram", int, test_psram, TEST_SIZE);
		if (test_psram){
			printf("Allocated PSRAM space\n");
		}

	    printf("RAM Test\n");
	    runTest(test_ram, TEST_SIZE);
		testSHA(test_ram, TEST_SIZE);

	    printf("PSRAM Test\n");
	    runTest(test_psram, TEST_SIZE);
	    testSHA(test_psram, TEST_SIZE);



	    while (true) {
	        sleep_ms(3000);
	        printf("Hello\n");
	    }
}
