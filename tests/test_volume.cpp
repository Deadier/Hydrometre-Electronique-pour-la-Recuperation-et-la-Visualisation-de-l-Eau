#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "../src/volume.h"

TEST_CASE("calculateVolume computes expected volume", "[volume]") {
    using Catch::Approx;
    REQUIRE(calculateVolume(200) == Approx(0.0f));
    REQUIRE(calculateVolume(150) == Approx(342073.363f).epsilon(0.001f));
    REQUIRE(calculateVolume(100) == Approx(1894124.854f).epsilon(0.001f));
    REQUIRE(calculateVolume(0) == Approx(6349736.806f).epsilon(0.001f));
}
