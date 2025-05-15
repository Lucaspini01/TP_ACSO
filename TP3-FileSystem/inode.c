#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"


/**
 * TODO
 */
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inode_out) {
    int maxInodes = fs->superblock.s_isize * 16;
    if (inumber <= 0 || inumber >= maxInodes) return -1;

    int sectorOffset = (inumber - 1) / 16;
    int indexInSector = (inumber - 1) % 16;

    char blockData[DISKIMG_SECTOR_SIZE];
    int sector = INODE_START_SECTOR + sectorOffset;

    if (diskimg_readsector(fs->dfd, sector, blockData) != DISKIMG_SECTOR_SIZE)
        return -1;

    struct inode *blockInodes = (struct inode *) blockData;
    *inode_out = blockInodes[indexInSector];
    return 0;
}


/**
 * TODO
 */
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inodePtr, int blockNum) {
    if (!(inodePtr->i_mode & ILARG)) {
        if (blockNum < 0 || blockNum >= 8) return -1;
        return inodePtr->i_addr[blockNum];
    }

    if (blockNum < 1792) {
        int indirIdx = blockNum / 256;
        int offset = blockNum % 256;

        uint16_t buffer[256];
        if (diskimg_readsector(fs->dfd, inodePtr->i_addr[indirIdx], buffer) != DISKIMG_SECTOR_SIZE)
            return -1;

        return buffer[offset];
    }

    int adjusted = blockNum - 1792;
    int outerIdx = adjusted / 256;
    int innerIdx = adjusted % 256;

    uint16_t outer[256];
    if (diskimg_readsector(fs->dfd, inodePtr->i_addr[7], outer) != DISKIMG_SECTOR_SIZE)
        return -1;

    uint16_t inner[256];
    if (diskimg_readsector(fs->dfd, outer[outerIdx], inner) != DISKIMG_SECTOR_SIZE)
        return -1;

    return inner[innerIdx];
}


int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
