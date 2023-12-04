// SPCX-License-Identifier: MIT

#define _LARGEFILE64_SOURCE
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/module.h>
#include <sys/syscall.h>

#include "file_mem.h"

const char * __progname;

static int usage(int argc, const char * argv[]);

#define METHOD_SYSTEM 0
#define METHOD_INIT_MODULE 1
#define METHOD_FINIT_MODULE 2
int opt_method = METHOD_SYSTEM; // 0:system() 1:init_module 2:finit_module
int opt_file = 0;

size_t rfsz = 0;
const char * rfname = NULL;

static int parse_args(int argc, const char * argv[]) {
    int ret = EXIT_SUCCESS;
    int i;

    printf("argv[%d]:%s"NL, 0, argv[0]);
    for (i=1; i<argc; i++) {
        const char * arg = argv[i];
        if (*arg == '-') {
            if ((strcmp(arg, "-h") == 0) || (strcmp(arg, "--help") == 0)) {
                return usage(argc, argv);
            } else if ((strcmp(arg, "-m") == 0) || (strcmp(arg, "--method") == 0)) {
                if ((i+1)<argc) {
                    i++;
                    opt_method = atoi(argv[i]);
                } else return usage(argc, argv);
            } else if (strcmp(arg, "--system") == 0) {
                opt_method = METHOD_SYSTEM;
                printf("Set load modlue method: insmod command via system()"NL);
            } else if (strcmp(arg, "--init_module") == 0) {
                opt_method = METHOD_INIT_MODULE;
                printf("Set load modlue method: init_module()"NL);
            } else if (strcmp(arg, "--finit_module") == 0) {
                opt_method = METHOD_FINIT_MODULE;
                printf("Set load modlue method: finit_module()"NL);
            } else {
                printf("Unknown argument argv[%d]:%s"NL, 0, argv[i]);
                return -EXIT_FAILURE;
            }
        } else if (!opt_file) {
            rfname = argv[i];

            if (rfname && rfname[0]) {
                struct stat sb;
                ret = stat(rfname, &sb);
                if (ret != 0) {
                    printf("%s: Cannot stat file '%s'!"NL, argv[0], rfname);
                    return -EXIT_FAILURE;
                }
                if (S_ISDIR(sb.st_mode)) {
                    printf("%s: file '%s' is not a file!"NL, argv[0], rfname);
                    return -EXIT_FAILURE;
                }
                rfsz = sb.st_size;
            } else {
                return usage(argc, argv);
            }
            opt_file = 1;
            ret = i+1;
            break;
        } else {
            // printf("Unknown argument argv[%d]:%s"NL, 0, argv[i]);
            ret = i;
            break;
        }
    }

    if (opt_file == 0) {
        return usage(argc, argv);
    }

    printf("FILE=%s(0x%lX)"NL, rfname, rfsz);

    return ret;
}
static int catvsprintf(char *s, const char *fmt, va_list ap) {
    char *p = s + strlen(s);
    return vsprintf(p, fmt, ap);
}
static int catsprintf(char *s, const char *fmt, ...) {
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = catvsprintf(s, fmt, ap);
    va_end(ap);
    return ret;
}
int main(int argc, const char * argv[]) {
    int ret = 0;
    int optind;
    char line[1024];
    char param_values[1024];
    int i;

    __progname = argv[0];
    if ((ret = parse_args(argc, argv)) < EXIT_SUCCESS) {
        return ret;
    }
    optind = ret;

    if (!rfname || !rfname[0]) {
        return usage(argc, argv);
    }
    sprintf(line, "sudo insmod %s", rfname);
    param_values[0] = '\0';

	printf("optind=%d argc=%d"NL, optind, argc);
    for (i=optind; i<argc; i++) {
        printf("argv[%d] = \"%s\""NL, i, argv[i]);
        catsprintf(line, " %s", argv[i]);
        catsprintf(param_values, "%s%s", (i>optind) ? " " : "", argv[i]);
    }

    if (opt_method == METHOD_SYSTEM) {
        printf("line = \"%s\""NL, line);
        ret = system(line);
        printf("ret = %d"NL, ret);
    } else if (opt_method == METHOD_INIT_MODULE) {
        void * module_image;
        size_t len;
        if ((module_image = file_ldmem(rfname, &len)) == NULL) {
            return -1;
        }
        ret = syscall(SYS_init_module, module_image, len, param_values);
        printf("ret = %d"NL, ret);
        free(module_image);
    } else if (opt_method == METHOD_FINIT_MODULE) {
        int fd;
        if ((fd = open(rfname, O_RDONLY)) < 0) {
            printf("%s: Cannot open file to read '%s'"NL, __progname, rfname);
            return -1;
        }
        printf("fd = %d"NL, fd);
        ret = syscall(SYS_finit_module
        // ret = syscall(__NR_finit_module
                , fd, param_values
                , 0
                | MODULE_INIT_IGNORE_MODVERSIONS   // Ignore symbol version hashes.
                | MODULE_INIT_IGNORE_VERMAGIC      // Ignore kernel version magic.
                );
        printf("ret = %d"NL, ret);
        close(fd);
    }

    return EXIT_SUCCESS;
}

static int usage(int argc, const char * argv[]) {
    printf(
            "usage: %s [OPTIONS] CPU ADDRESS FILE"NL
            ""NL
            "Load module file (*.ko)."NL
            ""NL
            "FILE               module image file (*.ko)"NL
            ""NL
            "OPTIONS"NL
            "-h, --help         Show this message"NL
            "--system           Set load modlue method: insmod command via system()"NL
            "--init_module      Set load modlue method: init_module()"NL
            "--finit_module     Set load modlue method: finit_module()"NL
            ""NL
            , argv[0]);
    fflush(stdout);
    return -1;
}

