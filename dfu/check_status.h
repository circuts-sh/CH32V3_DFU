#ifndef CHECK_STATUS_H
#define CHECK_STATUS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool check_fw();
uint8_t check_status();

#ifdef __cplusplus
}
#endif

#endif // CHECK_STATUS_H