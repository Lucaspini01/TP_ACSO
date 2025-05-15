#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

/**
 * TODO
 */
int directory_findname(struct unixfilesystem *fs, const char *targetName, int dirInum, struct direntv6 *result) {
  struct inode dirInode;
  if (inode_iget(fs, dirInum, &dirInode) < 0) return -1;

  if (!(dirInode.i_mode & IALLOC) || (dirInode.i_mode & IFMT) != IFDIR)
      return -1;

  int totalSize = inode_getsize(&dirInode);
  int blocks = (totalSize + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;

  char sectorBuf[DISKIMG_SECTOR_SIZE];

  for (int b = 0; b < blocks; b++) {
      int valid = file_getblock(fs, dirInum, b, sectorBuf);
      if (valid < 0) return -1;

      int entries = valid / sizeof(struct direntv6);
      struct direntv6 *entryArray = (struct direntv6 *) sectorBuf;

      for (int i = 0; i < entries; i++) {
          if (strncmp(entryArray[i].d_name, targetName, 14) == 0) {
              *result = entryArray[i];
              return 0;
          }
      }
  }

  return -1;
}


