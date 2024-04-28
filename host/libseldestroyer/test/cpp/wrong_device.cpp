#include "libseldestroyer.h"

#include "test.hpp"


int main() {

    lsd_obj_t lobj = lsd_init();

    lsd_return_val_t val = lsd_connect(lobj, "/dev/this_device_does_not_exist");

    ASSERT_EQ(val,LSD_UNABLE_TO_CONNECT);
    ASSERT_EQ(lsd_get_error(lobj), std::string("No such file or directory"));

    return 0;
}