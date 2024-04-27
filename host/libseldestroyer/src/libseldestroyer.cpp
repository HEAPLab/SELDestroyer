/*
 * Copyright 2024 Politecnico di Milano
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include "libseldestroyer.h"
#include "session.hpp"
#include "exception.hpp"

#include <string>

#define SPTR_CONV(x) ((libdestroyer::Session*)x)

#define EXCEPTION_PROTECT_START try {


#define EXCEPTION_PROTECT_END } catch(const LSDException &lsde) { \
                        SPTR_CONV(session)->last_error = lsde.what();\
                        return lsde.get_error_code();\
                    } catch(...) {\
                        SPTR_CONV(session)->last_error = "Unknown error.";\
                        return LSD_GENERIC_ERROR;\
                    }

lsd_obj_t lsd_init(void) {
    return new (std::nothrow) libdestroyer::Session;
}

void lsd_close(lsd_obj_t session) {
    if(session != nullptr) {
        delete SPTR_CONV(session);
    }
}


lsd_return_val_t lsd_connect(lsd_obj_t session, const char* device_name) {
    EXCEPTION_PROTECT_START

    EXCEPTION_PROTECT_END
}