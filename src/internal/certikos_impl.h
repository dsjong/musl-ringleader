#ifndef _CERTIKOS_IMPL_H
#define _CERTIKOS_IMPL_H

#define DEFAULT_CONSOLE_ID 0u

#include <certikos.h>
#include "stdio_impl.h"

static FILE certikos_stdout = {.flags = F_ENCLAVE_TERMINAL};

struct ringleader* get_ringleader(void);
void *get_rl_shmem(void);

#endif