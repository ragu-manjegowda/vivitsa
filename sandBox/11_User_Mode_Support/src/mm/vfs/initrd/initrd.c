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
/* Global variable to keep track of inodes */
static uint32_t g_INODE;
/* Name of root directory */
static const char g_ROOT_DIR[2] = "/";

static uint32_t initrd_read(fs_node_t *node, uint32_t offset, uint32_t size,
                          uint8_t *buffer) {
  initrd_file_header_t header = g_FILE_HEADERS[node->inode];
  if (offset > header.length)
    return 0;
  if (offset + size > header.length)
    size = header.length - offset;
  custom_memcpy(buffer, (uint8_t *)(header.offset + offset), size);
  return size;
}

static uint8_t initrd_readdir(fs_node_t *node, uint32_t index,
                            fs_node_t *directory) {
  if (directory == 0) {
    return 1;
  }

  if (index >= node->size) {
    return 1;
  }

  custom_memcpy((uint8_t *)directory, (uint8_t *)&node->contents[index],
                sizeof(fs_node_t));
  return 0;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name) {
  for (uint32_t i = 0; i < node->size; i++)
    if (!custom_strcmp(name, node->contents[i].name, 1))
      return &node->contents[i];
  return 0;
}

/* Initialise the root directory, /dev directory and populate the them */
void initialise_initrd(uint32_t location) {
  uint32_t numOfNodes = *((uint32_t *)location);
  initrd_file_header_t *fileHeaderPtr =
      (initrd_file_header_t *)(location + sizeof(uint32_t));

  /* Allocate size of 64 global file node headers we can expand it later */
  g_FILE_HEADERS =
      (initrd_file_header_t *)kmalloc(sizeof(initrd_file_header_t) * LEN_64);

  /* Initialise the root directory */
  g_INITRD_ROOT_DIR = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  /* Initialise the /dev directory, we mount our fs here  */
  fs_node_t *initrdDevDir = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  /* Initialise /dev nodes */
  fs_node_t *initrdDevNodes =
      (fs_node_t *)kmalloc(sizeof(fs_node_t) * numOfNodes);

  /* Populate /root directory */
  custom_strcpy(g_INITRD_ROOT_DIR->name, &g_ROOT_DIR[0]);
  g_INITRD_ROOT_DIR->mask = g_INITRD_ROOT_DIR->uid = g_INITRD_ROOT_DIR->gid = 0;
  g_INITRD_ROOT_DIR->inode = g_INODE++;
  g_INITRD_ROOT_DIR->length = 0;
  g_INITRD_ROOT_DIR->type = FS_DIRECTORY;
  g_INITRD_ROOT_DIR->read = g_INITRD_ROOT_DIR->write = 0;
  g_INITRD_ROOT_DIR->open = 0;
  g_INITRD_ROOT_DIR->close = 0;
  g_INITRD_ROOT_DIR->readdir = &initrd_readdir;
  g_INITRD_ROOT_DIR->finddir = &initrd_finddir;
  /* For now we will only have /dev in our /root directory */
  g_INITRD_ROOT_DIR->contents = initrdDevDir;
  g_INITRD_ROOT_DIR->size = 1;
  g_FILE_HEADERS[g_INITRD_ROOT_DIR->inode].magic = 0xCA;
  custom_strcpy(g_FILE_HEADERS[g_INITRD_ROOT_DIR->inode].name,
                g_INITRD_ROOT_DIR->name);
  g_FILE_HEADERS[g_INITRD_ROOT_DIR->inode].type = FS_DIRECTORY;
  g_FILE_HEADERS[g_INITRD_ROOT_DIR->inode].offset = 0;
  g_FILE_HEADERS[g_INITRD_ROOT_DIR->inode].length = 0;

  /* Populate /dev directory */
  custom_strcpy(initrdDevDir->name, "dev");
  initrdDevDir->mask = initrdDevDir->uid = initrdDevDir->gid = 0;
  initrdDevDir->inode = g_INODE++;
  initrdDevDir->length = 0;
  initrdDevDir->type = FS_DIRECTORY;
  initrdDevDir->read = initrdDevDir->write = 0;
  initrdDevDir->open = 0;
  initrdDevDir->close = 0;
  initrdDevDir->readdir = &initrd_readdir;
  initrdDevDir->finddir = &initrd_finddir;
  /* /dev will have files loaded from initrd */
  initrdDevDir->contents = initrdDevNodes;
  initrdDevDir->size = numOfNodes;
  g_FILE_HEADERS[initrdDevDir->inode].magic = 0xCA;
  custom_strcpy(g_FILE_HEADERS[initrdDevDir->inode].name, initrdDevDir->name);
  g_FILE_HEADERS[initrdDevDir->inode].type = FS_DIRECTORY;
  g_FILE_HEADERS[initrdDevDir->inode].offset = 0;
  g_FILE_HEADERS[initrdDevDir->inode].length = 0;

  /*
   * Copy initrd file header to global variable, although this is duplicate this
   * simplifies the maintainance of file system as we keep on adding file nodes
   * during run time
   */
  custom_memcpy((uint8_t *)&g_FILE_HEADERS[g_INODE], (uint8_t *)fileHeaderPtr,
                sizeof(initrd_file_header_t) * numOfNodes);

  for (uint32_t i = 0; i < numOfNodes; i++) {
    /*
     * Edit the file's header - currently it holds the file offset
     * relative to the start of the ramdisk. We want it relative to the start
     * of memory
     */
    g_FILE_HEADERS[g_INODE].offset += location;

    /* Populate nodes */
    custom_strcpy(initrdDevNodes[i].name, &(g_FILE_HEADERS[g_INODE].name[0]));
    initrdDevNodes[i].mask = initrdDevNodes[i].uid = initrdDevNodes[i].gid = 0;
    initrdDevNodes[i].length = g_FILE_HEADERS[g_INODE].length;
    initrdDevNodes[i].inode = g_INODE++;
    initrdDevNodes[i].type = FS_FILE;
    initrdDevNodes[i].read = &initrd_read;
    initrdDevNodes[i].write = 0;
    initrdDevNodes[i].readdir = 0;
    initrdDevNodes[i].finddir = 0;
    initrdDevNodes[i].open = 0;
    initrdDevNodes[i].close = 0;
    initrdDevNodes[i].contents = 0;
  }
}
