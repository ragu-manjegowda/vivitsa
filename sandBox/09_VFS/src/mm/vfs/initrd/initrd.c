#include "initrd.h"
#include <helpers.h>
#include <kheap.h>

/*
 * Global list of file headers, based on inode we will index this list and get
 * the contents
 */
initrd_file_header_t *g_FILE_HEADERS;
/* Root directory node */
fs_node_t *g_INITRD_ROOT_DIR;
/* Directory node for /dev, to mount devfs later on */
fs_node_t *g_INITRD_DEV_DIR;
/* List of all nodes in /dev directory */
fs_node_t *g_INITRD_DEV_NODES;
/* Number of nodes in /dev directory */
u32int g_NUM_OF_INITRD_DEV_NODES;

static u32int initrd_read(fs_node_t *node, u32int offset, u32int size,
                          u8int *buffer) {
  initrd_file_header_t header = g_FILE_HEADERS[node->inode];
  if (offset > header.length)
    return 0;
  if (offset + size > header.length)
    size = header.length - offset;
  custom_memcpy(buffer, (u8int *)(header.offset + offset), size);
  return size;
}

static u8int initrd_readdir(fs_node_t *node, u32int index,
                            fs_node_t *directory) {
  if (directory == 0) {
    return 1;
  }

  if (node == g_INITRD_ROOT_DIR && index == 0) {
    custom_strcpy(directory->name, "dev");
    directory->name[3] = 0;
    directory->inode = 0;
    return 0;
  }

  if (index - 1 >= g_NUM_OF_INITRD_DEV_NODES) {
    return 1;
  }

  custom_strcpy(directory->name, g_INITRD_DEV_NODES[index - 1].name);
  directory->name[custom_strlen(g_INITRD_DEV_NODES[index - 1].name)] = 0;
  directory->inode = g_INITRD_DEV_NODES[index - 1].inode;
  return 0;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name) {
  if (node == g_INITRD_ROOT_DIR && !custom_strcmp(name, "dev", 1))
    return g_INITRD_DEV_DIR;

  u32int i;
  for (i = 0; i < g_NUM_OF_INITRD_DEV_NODES; i++)
    if (!custom_strcmp(name, g_INITRD_DEV_NODES[i].name, 1))
      return &g_INITRD_DEV_NODES[i];
  return 0;
}

/* Initialise the root directory, /dev directory and populate the them */
fs_node_t *initialise_initrd(u32int location) {
  u32int *numOfFilesPtr = (u32int *)location;
  g_FILE_HEADERS = (initrd_file_header_t *)(location + sizeof(u32int));
  g_NUM_OF_INITRD_DEV_NODES = *numOfFilesPtr;

  /* Initialise the root directory */
  g_INITRD_ROOT_DIR = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  /* Initialise the /dev directory, we mount our fs here  */
  g_INITRD_DEV_DIR = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  /* Initialise /dev nodes */
  g_INITRD_DEV_NODES =
      (fs_node_t *)kmalloc(sizeof(fs_node_t) * g_NUM_OF_INITRD_DEV_NODES);

  /* Populate /root directory */
  custom_strcpy(g_INITRD_ROOT_DIR->name, "initrd");
  g_INITRD_ROOT_DIR->mask = g_INITRD_ROOT_DIR->uid = g_INITRD_ROOT_DIR->gid =
      g_INITRD_ROOT_DIR->inode = g_INITRD_ROOT_DIR->length = 0;
  g_INITRD_ROOT_DIR->type = FS_DIRECTORY;
  g_INITRD_ROOT_DIR->read = g_INITRD_ROOT_DIR->write = 0;
  g_INITRD_ROOT_DIR->open = 0;
  g_INITRD_ROOT_DIR->close = 0;
  g_INITRD_ROOT_DIR->readdir = &initrd_readdir;
  g_INITRD_ROOT_DIR->finddir = &initrd_finddir;
  /* For now we will only have /dev in our /root directory */
  g_INITRD_ROOT_DIR->contents = g_INITRD_DEV_DIR;
  g_INITRD_ROOT_DIR->size = 1;

  /* Populate /dev directory */
  custom_strcpy(g_INITRD_DEV_DIR->name, "dev");
  g_INITRD_DEV_DIR->mask = g_INITRD_DEV_DIR->uid = g_INITRD_DEV_DIR->gid =
      g_INITRD_DEV_DIR->inode = g_INITRD_DEV_DIR->length = 0;
  g_INITRD_DEV_DIR->type = FS_DIRECTORY;
  g_INITRD_DEV_DIR->read = g_INITRD_DEV_DIR->write = 0;
  g_INITRD_DEV_DIR->open = 0;
  g_INITRD_DEV_DIR->close = 0;
  g_INITRD_DEV_DIR->readdir = &initrd_readdir;
  g_INITRD_DEV_DIR->finddir = &initrd_finddir;
  /* /dev will have files loaded from initrd */
  g_INITRD_DEV_DIR->contents = g_INITRD_DEV_NODES;
  g_INITRD_DEV_DIR->size = g_NUM_OF_INITRD_DEV_NODES;

  for (u32int i = 0; i < g_NUM_OF_INITRD_DEV_NODES; i++) {
    /*
     * Edit the file's header - currently it holds the file offset
     * relative to the start of the ramdisk. We want it relative to the start
     * of memory
     */
    g_FILE_HEADERS[i].offset += location;

    /* Populate nodes */
    custom_strcpy(g_INITRD_DEV_NODES[i].name, &(g_FILE_HEADERS[i].name[0]));
    g_INITRD_DEV_NODES[i].mask = g_INITRD_DEV_NODES[i].uid =
        g_INITRD_DEV_NODES[i].gid = 0;
    g_INITRD_DEV_NODES[i].length = g_FILE_HEADERS[i].length;
    g_INITRD_DEV_NODES[i].inode = i;
    g_INITRD_DEV_NODES[i].type = FS_FILE;
    g_INITRD_DEV_NODES[i].read = &initrd_read;
    g_INITRD_DEV_NODES[i].write = 0;
    g_INITRD_DEV_NODES[i].readdir = 0;
    g_INITRD_DEV_NODES[i].finddir = 0;
    g_INITRD_DEV_NODES[i].open = 0;
    g_INITRD_DEV_NODES[i].close = 0;
    g_INITRD_DEV_NODES[i].contents = 0;
  }
  return g_INITRD_ROOT_DIR;
}
