#include "libseldestroyer.h"

#include "test.hpp"


int main() {

    lsd_debug_mode(true);

    lsd_obj_t lobj = lsd_init();

    lsd_return_val_t val = lsd_connect(lobj, "/dev/ttyUSB0");

    std::cout << lsd_get_error(lobj) << std::endl;

    ASSERT_EQ(val,LSD_OK);

    return 0;
}