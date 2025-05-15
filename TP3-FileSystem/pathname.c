
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int pathname_lookup(struct unixfilesystem *fs, const char *path) {
    if (!path || path[0] != '/') return -1;

    int currentInum = ROOT_INUMBER;
    const char *cursor = path + 1;

    char segment[15];
    while (*cursor != '\0') {
        int len = 0;
        while (cursor[len] != '/' && cursor[len] != '\0') {
            if (len >= 14) return -1;
            segment[len] = cursor[len];
            len++;
        }
        segment[len] = '\0';

        struct direntv6 found;
        if (directory_findname(fs, segment, currentInum, &found) < 0)
            return -1;

        currentInum = found.d_inumber;

        cursor += len;
        if (*cursor == '/') cursor++;
    }

    return currentInum;
}

