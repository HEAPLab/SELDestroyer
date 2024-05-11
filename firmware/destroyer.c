#include "destroyer.h"

destroyer_data_t destroyer_data;


void destroyer_init(void) {
    destroyer_data.I_limit = 1234;
    destroyer_data.count = 123;
    destroyer_data.dut_is_active = true;
}