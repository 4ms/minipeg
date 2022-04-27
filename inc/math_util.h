
static inline uint32_t diff32(uint32_t a, uint32_t b) {
	if (a > b)
		return (a - b);
	else
		return (b - a);
}

static inline uint16_t diff(uint16_t a, uint16_t b) {
	if (a > b)
		return (a - b);
	else
		return (b - a);
}
