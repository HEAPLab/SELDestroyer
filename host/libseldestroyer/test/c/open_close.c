#include "libseldestroyer.h"

#include <stdio.h>

int main() {

    lsd_obj_t lobj = lsd_init();

    lsd_close(lobj);

    return 0;
}