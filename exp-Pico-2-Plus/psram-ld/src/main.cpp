/**
 * Jon Durrant.
 *
 * Blink STATUS LED
 */

#include "pico/stdlib.h"
#include <cstdlib>
#include "hardware/regs/xip.h"
#include "hardware/structs/xip.h"
#include "pico/sha256.h"
extern "C"{
#include "pico/status_led.h"
}
#include <cstdio>

#define DELAY 500 // in microseconds
#define TEST_SIZE (1024*32)

__attribute__((section(".psram"))) uint8_t test_psramA[0x40000];
__attribute__((section(".psram"))) uint8_t test_psramB[0x40000];


void runTest(int *p, size_t len){
	uint64_t start = to_us_since_boot (get_absolute_time());
	for (int i=0; i < len; i++){
		p[i] = i;
		if (p[i] != i){
			printf("RAM Write failed\n");
			return;
		}
	}

	int total =0;
	for (int i=0; i < len; i++){
		total = p[i] ;
	}
	uint64_t end = to_us_since_boot (get_absolute_time());
	uint64_t ms = end - start;
	printf("Completed in %llu us\n", ms);
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
		stdio_init_all();

		gpio_set_function(47, GPIO_FUNC_XIP_CS1); // CS for PSRAM
		xip_ctrl_hw->ctrl|=XIP_CTRL_WRITABLE_M1_BITS;

		sleep_ms(2000);
		printf("Start\n");
	 	bool rc = status_led_init();
	    hard_assert(rc);
	    status_led_set_state(true);

	    printf("test_psram assigned as 0x%lX  and 0x%lX\n\n", test_psramA, test_psramB);

	    printf("RAM Test\n");
	    int * ram = (int *)malloc(sizeof(int) * TEST_SIZE );
	    runTest(ram, TEST_SIZE);
	    testSHA(ram, TEST_SIZE);

	    printf("PSRAM Test\n");
	    int * psram = (int *)0x11000000;
	    runTest(psram, TEST_SIZE);
	    testSHA(psram, TEST_SIZE);


	    while (true) {
	        status_led_set_state(true);
	        sleep_ms(DELAY);
	        status_led_set_state(false);
	        sleep_ms(DELAY);
	    }
	    status_led_deinit();
}
