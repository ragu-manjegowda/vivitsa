#ifndef INCLUDE_INITRD_H
#define INCLUDE_INITRD_H

#pragma once
#include <fs.h>
#include <types.h>

typedef struct {
  /* Magic number, for error checking */
  uint8_t magic;
  /* Filename */
  int8_t name[LEN_64];
  /* Node type, file = 1, directory = 2 */
  int8_t type;
  /* Offset in the initrd where file starts */
  uint32_t offset;
  /* Length of the file */
  uint32_t length;
} initrd_file_header_t;

/** initialise_initrd:
 *  Initialises the initial ramdisk.
 *
 *  @param  location Address of the multiboot module
 */
void initialise_initrd(uint32_t location);

#endif /* INCLUDE_INITRD_H */
