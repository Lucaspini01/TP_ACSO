#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

/**
 * TODO
 */
int file_getblock(struct unixfilesystem *fs, int inumber, int logicalBlock, void *outputBuf) {
    struct inode currentInode;
    if (inode_iget(fs, inumber, &currentInode) < 0) return -1;

    int totalSize = inode_getsize(&currentInode);
    int totalBlocks = (totalSize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;

    if (logicalBlock >= totalBlocks) return 0;

    int diskBlockNum = inode_indexlookup(fs, &currentInode, logicalBlock);
    if (diskBlockNum <= 0) return -1;

    int bytesRead = diskimg_readsector(fs->dfd, diskBlockNum, outputBuf);
    if (bytesRead < 0) return -1;

    int remaining = totalSize - logicalBlock * DISKIMG_SECTOR_SIZE;
    return (remaining < DISKIMG_SECTOR_SIZE) ? remaining : DISKIMG_SECTOR_SIZE;
}


