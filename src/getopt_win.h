/*
 * Minimal POSIX getopt() for Windows, force-included (/FI) into qjsc.c when
 * building tjsc with MSVC/clang-cl (the MSVC CRT has no <unistd.h>/getopt).
 * qjsc only uses plain short options ("ho:p:n:ms"), so this small public-domain
 * style implementation suffices. Header-only + static: tjsc is a single-TU exe.
 */
#pragma once
#if defined(_WIN32)
#include <string.h>
#include <stdio.h>

static char *optarg = NULL;
static int optind = 1;
static int opterr = 1;
static int optopt = 0;

static int getopt(int argc, char *const argv[], const char *optstring) {
    static int optpos = 1;
    if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0') {
        return -1;
    }
    if (strcmp(argv[optind], "--") == 0) {
        optind++;
        return -1;
    }
    int opt = (unsigned char)argv[optind][optpos];
    const char *match = strchr(optstring, opt);
    if (opt == ':' || match == NULL) {
        if (opterr) {
            fprintf(stderr, "%s: illegal option -- %c\n", argv[0], opt);
        }
        optopt = opt;
        if (argv[optind][++optpos] == '\0') { optind++; optpos = 1; }
        return '?';
    }
    if (match[1] == ':') {           /* option takes an argument */
        if (argv[optind][optpos + 1] != '\0') {
            optarg = &argv[optind][optpos + 1];
            optind++;
        } else if (optind + 1 < argc) {
            optarg = argv[optind + 1];
            optind += 2;
        } else {
            optopt = opt;
            optind++;
            optpos = 1;
            return (optstring[0] == ':') ? ':' : '?';
        }
        optpos = 1;
    } else {                         /* option without argument */
        if (argv[optind][++optpos] == '\0') { optind++; optpos = 1; }
    }
    return opt;
}
#endif
