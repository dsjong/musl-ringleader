#include <unistd.h>
#include "syscall.h"

#ifdef _CERTIKOS_
#include "certikos_impl.h"
#include "ringleader.h"
#include <string.h>
#endif

ssize_t write(int fd, const void *buf, size_t count)
{
	#ifndef _CERTIKOS_
	return syscall_cp(SYS_write, fd, buf, count);
	#else
	struct ringleader *rl = get_ringleader();
    //TODO how tight can this bound be
	void *shmem = (count + 0x100 <= SHMEM_SIZE) ? get_rl_shmem_singleton() :
		alloc_new_rl_shmem(count + 0x100);
	if(shmem == NULL) return __syscall_ret(-ENOMEM);

	memcpy(shmem, buf, count);

	int32_t id = ringleader_prep_write(rl, fd, shmem, count, -1);
	ringleader_set_user_data(rl, id, (void *) WRITE_COOKIE);
	ringleader_submit(rl);

	struct io_uring_cqe *cqe = ringleader_get_cqe(rl);
	if((uint64_t) cqe->user_data == WRITE_COOKIE){
		__s32 ret = cqe->res;
		ringleader_consume_cqe(rl, cqe);
		return __syscall_ret(ret);
	} else {
		ringleader_consume_cqe(rl, cqe);
		certikos_puts("Did not get expected ringleader write completion token");
		return __syscall_ret(-EIO);
	}
	#endif
}
