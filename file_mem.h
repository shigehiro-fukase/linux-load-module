// SPCX-License-Identifier: MIT
#ifndef ___FILE_MEM_H___
#define ___FILE_MEM_H___

// #define _LARGEFILE64_SOURCE
#include <features.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef NL
#define NL "\n"
#endif

#if defined(__USE_FILE_OFFSET64)
#define OFF_T off64_t
#define LSEEK(fd, offset, whence) lseek64(fd, offset, whence)
#else
#define OFF_T off_t
#define LSEEK(fd, offset, whence) lseek(fd, offset, whence)
#endif
static inline OFF_T fd_get_size(int fd) {
    OFF_T pos, size;
    if (fd < 0) return -1;
    pos = LSEEK(fd, 0, SEEK_CUR); // current pos
    LSEEK(fd, 0, SEEK_SET);
    size = LSEEK(fd, 0, SEEK_END);
    LSEEK(fd, pos, SEEK_SET); // rewind to pos
    return size;
}
static inline void* file_ldmem(const char * path, size_t * rlen) {
    int ret = 0;
    int fd = -1;
    OFF_T fsz;
    void * va = NULL;
    ssize_t n;
    extern const char * __progname;

    if ((fd = open(path, O_RDONLY)) < 0) {
        printf("%s: Cannot open file to read '%s'"NL, __progname, path);
        ret = -1;
        goto L_RETURN;
    }

    fsz = fd_get_size(fd);
    if ((va = (void*) malloc(fsz)) == NULL) {
        printf("%s: Cannot allocate memory (%lu bytes)"NL, __progname, fsz);
        ret = -1;
        goto L_RETURN;
    }

    n = read(fd, va, fsz);
    printf("%s: Load %lu bytes."NL, __progname, n);

    if (rlen) {
        *rlen = n;
    }

L_RETURN:
    if (fd >= 0) close(fd);
    return va;
}

#endif /* ___FILE_MEM_H___ */

