#include "core/application_info.h"
#include <cstdlib>
#include <iostream>

int main() {
    std::cout << im_cpp::core::application_name() << '\n';
    return EXIT_SUCCESS;
}
