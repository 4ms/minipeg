#include "stm32xx.h"
#include <stdint.h>

void *memcpy(void *dest, const void *src, unsigned int n) {
	char *dp = (char *)dest;
	const char *sp = (const char *)src;
	while (n--)
		*dp++ = *sp++;
	return dest;
}

typedef void (*EntryPoint)(void);
void jump_to(uint32_t address) {
	uint32_t application_address = *(volatile uint32_t *)(address + 4);
	EntryPoint application = (EntryPoint)(application_address);
	__set_MSP(*(volatile uint32_t *)address);
	application();
}
