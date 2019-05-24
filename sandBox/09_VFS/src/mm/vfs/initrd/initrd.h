#ifndef INCLUDE_INITRD_H
#define INCLUDE_INITRD_H

#pragma once
#include <fs.h>
#include <types.h>

typedef struct {
  /* Magic number, for error checking */
  u8int magic;
  /* Filename */
  s8int name[LEN_64];
  /* Node type, file = 1, directory = 2 */
  s8int type;
  /* Offset in the initrd where file starts */
  u32int offset;
  /* Length of the file */
  u32int length;
} initrd_file_header_t;

/** initialise_initrd:
 *  Initialises the initial ramdisk.
 *
 *  @param  location Address of the multiboot module
 */
void initialise_initrd(u32int location);

#endif /* INCLUDE_INITRD_H */
