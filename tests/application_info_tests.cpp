#include "core/application_info.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("application_name_Should_ReturnImCpp") {
    REQUIRE(im_cpp::core::application_name() == "im-cpp");
}
