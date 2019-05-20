#include "initrd.h"
#include <helpers.h>
#include <kheap.h>

/* The list of file headers */
initrd_file_header_t *g_fileHeaders;
/* Root directory node */
fs_node_t *initrd_root;
/* Directory node for /dev, to mount devfs later on */
fs_node_t *initrd_dev;
// List of file nodes.
fs_node_t *root_nodes;
/* Number of file nodes */
u32int nroot_nodes;

/* Global variable to return directory */
dir_entry_t g_directory;

static u32int initrd_read(fs_node_t *node, u32int offset, u32int size,
                          u8int *buffer) {
  initrd_file_header_t header = g_fileHeaders[node->inode];
  if (offset > header.length)
    return 0;
  if (offset + size > header.length)
    size = header.length - offset;
  custom_memcpy(buffer, (u8int *)(header.offset + offset), size);
  return size;
}

static dir_entry_t *initrd_readdir(fs_node_t *node, u32int index) {
  if (node == initrd_root && index == 0) {
    custom_strcpy(g_directory.name, "dev");
    g_directory.name[3] = 0;
    g_directory.inode = 0;
    return &g_directory;
  }

  if (index - 1 >= nroot_nodes)
    return 0;
  custom_strcpy(g_directory.name, root_nodes[index - 1].name);
  g_directory.name[custom_strlen(root_nodes[index - 1].name)] = 0;
  g_directory.inode = root_nodes[index - 1].inode;
  return &g_directory;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name) {
  if (node == initrd_root && !custom_strcmp(name, "dev", 1))
    return initrd_dev;

  u32int i;
  for (i = 0; i < nroot_nodes; i++)
    if (!custom_strcmp(name, root_nodes[i].name, 1))
      return &root_nodes[i];
  return 0;
}

/* Initialise the root directory, /dev directory and populate the them */
fs_node_t *initialise_initrd(u32int location) {

  u32int *numOfFilesPtr = (u32int *)location;
  g_fileHeaders = (initrd_file_header_t *)(location + sizeof(u32int));

  /* Initialise the root directory */
  initrd_root = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  custom_strcpy(initrd_root->name, "initrd");
  initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode =
      initrd_root->length = 0;
  initrd_root->type = FS_DIRECTORY;
  initrd_root->read = initrd_root->write = 0;
  initrd_root->open = 0;
  initrd_root->close = 0;
  initrd_root->readdir = &initrd_readdir;
  initrd_root->finddir = &initrd_finddir;
  initrd_root->ptr = 0;

  /* Initialise the /dev directory, we mount our fs here  */
  initrd_dev = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  custom_strcpy(initrd_dev->name, "dev");
  initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode =
      initrd_dev->length = 0;
  initrd_dev->type = FS_DIRECTORY;
  initrd_dev->read = initrd_dev->write = 0;
  initrd_dev->open = 0;
  initrd_dev->close = 0;
  initrd_dev->readdir = &initrd_readdir;
  initrd_dev->finddir = &initrd_finddir;
  initrd_dev->ptr = 0;

  nroot_nodes = *numOfFilesPtr;
  root_nodes = (fs_node_t *)kmalloc(sizeof(fs_node_t) * nroot_nodes);

  // For every file...
  u32int i;
  for (i = 0; i < nroot_nodes; i++) {
    // Edit the file's header - currently it holds the file offset
    // relative to the start of the ramdisk. We want it relative to the start
    // of memory.
    g_fileHeaders[i].offset += location;
    // Create a new file node.
    custom_strcpy(root_nodes[i].name, &(g_fileHeaders[i].name[0]));
    root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
    root_nodes[i].length = g_fileHeaders[i].length;
    root_nodes[i].inode = i;
    root_nodes[i].type = FS_FILE;
    root_nodes[i].read = &initrd_read;
    root_nodes[i].write = 0;
    root_nodes[i].readdir = 0;
    root_nodes[i].finddir = 0;
    root_nodes[i].open = 0;
    root_nodes[i].close = 0;
    root_nodes[i].ptr = 0;
  }
  return initrd_root;
}
