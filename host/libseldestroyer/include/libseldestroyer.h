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

#ifndef LIBSELDESTROYER_H_
#define LIBSELDESTROYER_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum lsd_return_val_s {
    LSD_OK = 0,
    LSD_GENERIC_ERROR = 1,
    LSD_UNABLE_TO_CONNECT = 2
} lsd_return_val_t;

typedef void * lsd_obj_t;

lsd_obj_t lsd_init(void);

void lsd_debug_mode(bool is_enabled);

void lsd_close(lsd_obj_t session);

const char *lsd_get_error(lsd_obj_t session);

lsd_return_val_t lsd_connect(lsd_obj_t session, const char* device_name);

#ifdef __cplusplus
}
#endif

#endif