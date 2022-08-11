#include "doctest.h"
#include "env_update.h"
#include "flash_user.hh"
#include "pingable_env.h"

TEST_CASE("First test") {
	CHECK(true);
}
int32_t scale, offset, shift;

TEST_CASE("Get env_update to compile") {
	PingableEnvelope m;
	SystemSettings settings;

	// update_envelope(&m);
}
