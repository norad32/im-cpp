#include "core/application_info.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("applicationName_Should_ReturnImCpp")
{
    REQUIRE(im_cpp::core::applicationName() == "im-cpp");
}
