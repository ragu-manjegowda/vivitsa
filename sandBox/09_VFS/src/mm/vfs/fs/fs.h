#ifndef INCLUDE_FS_H
#define INCLUDE_FS_H

#pragma once
#include <types.h>

/* MAcros to identify the node type */
#define FS_FILE 0x01
#define FS_DIRECTORY 0x02
#define FS_CHARDEVICE 0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE 0x05
#define FS_SYMLINK 0x06
/* Skipping 0x07 so that 0x08 can be masked easily to checj if it is a mount */
#define FS_MOUNTPOINT 0x08

/* Forward declarations (for below callbacks)*/
struct fs_node;

/*
 * POSIX specific file specific callbacks APIs, every node will have this
 * functions supported
 */

/* read file */
typedef u32int (*read_type_t)(struct fs_node *, u32int, u32int, u8int *);

/* write to file */
typedef u32int (*write_type_t)(struct fs_node *, u32int, u32int, u8int *);

/* open file */
typedef s32int (*open_type_t)(struct fs_node *, u8int write);

/* close file */
typedef void (*close_type_t)(struct fs_node *);

/* read nth entry in the directory, return 0 if index is out of bound */
typedef u8int (*readdir_type_t)(struct fs_node *, u32int, struct fs_node *);

/* find the entry in directory, return 0 if not present */
typedef struct fs_node *(*finddir_type_t)(struct fs_node *, char *name);

/* File node struct */
typedef struct fs_node {
  /* Filename (limited to 64 characters) */
  s8int name[LEN_64];
  /* Permissions mask (unix-like mask - user, group, others) */
  u32int mask;
  /* Owning user */
  u32int uid;
  /* Owning group */
  u32int gid;
  /* Node type - directory/file/mounttype */
  u32int type;
  /* Device-specific - for filesystem to identify files (POSIX specific) */
  u32int inode;
  /* Size of the file, in bytes */
  u32int length;
  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir;
  finddir_type_t finddir;
  /* For MounPoint, directories and Symlinks, contents and size */
  struct fs_node *contents;
  u32int size;
} fs_node_t;

/*
 * POSIX specific file specific functions for file system to support
 */

/** read_fs:
 *  read the contents of the file specified by node.
 *
 *  @param node     Pointer to file node
 *  @param offset   Offset from where to start reading
 *  @param size     Size to read
 *  @param buffer   Pointer to a buffer to which the contents are to be
 * copied
 *  @return         Size of data read in number of bytes
 */
u32int read_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

/** write_fs:
 *  write to the file specified by node.
 *
 *  @param node         Pointer to file node
 *  @param offset       Offset from where to start writing
 *  @param size         Size to write
 *  @param buffer       Pointer to a buffer to which the contents are to be
 *                      copied
 */
u32int write_fs(fs_node_t *node, u32int offset, u32int size, u8int *buffer);

/** open_fs:
 *  open the file specified by node.
 *
 *  @param  node    Pointer to file node
 *  @param  offset  Offset from where to start writing
 *  @param  write   Flag to specify mode, 1 write, 0 read
 *  @return         file descriptor (a uniqure number) if success -1 otherwise
 */
s32int open_fs(fs_node_t *node, u8int write);

/** close_fs:
 *  close the file specified by node.
 *
 *  @param  node    Pointer to file node
 */
void close_fs(fs_node_t *node);

/** readdir_fs:
 *  read directory specified by node.
 *
 *  @param  node        Pointer to directory node
 *  @param  index       Index of entry in direcoty
 *  @param  directory   Pointer to directory entry at index if exist, return 0
 *                      if success 0 otherwise
 */
u8int readdir_fs(fs_node_t *node, u32int index, fs_node_t *directory);

/** findir_fs:
 *  if file/directory in the directory specified by node.
 *
 *  @param  node    Pointer to directory node
 *  @param  name    name of the directory or file to find
 *  @return         Pointer to file/directory node if exist 0 otherwise
 */
fs_node_t *finddir_fs(fs_node_t *node, s8int *name);

#endif /* INCLUDE_FS_H */
