#include "core/app_info.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("applicationName_Should_ReturnImCpp")
{
    REQUIRE(im_cpp::core::applicationName() == im_cpp::core::metadata::applicationName);
    REQUIRE(im_cpp::core::applicationVersion() == "0.1.0");
    REQUIRE_FALSE(im_cpp::core::applicationDescription().empty());
    REQUIRE_FALSE(im_cpp::core::applicationHomepage().empty());
    REQUIRE(im_cpp::core::applicationLicense() == "MIT");
    REQUIRE(im_cpp::core::applicationAuthor() == "norad32");
}
