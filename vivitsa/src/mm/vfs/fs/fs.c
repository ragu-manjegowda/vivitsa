#include "fs.h"

/* filesystem root */
fs_node_t *fs_root = 0;

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
  /* If node has a read callback, call it */
  if (node->read != 0) {
    return node->read(node, offset, size, buffer);
  } else {
    return 0;
  }
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
  /* If node has a write callback, call it */
  if (node->write != 0) {
    return node->write(node, offset, size, buffer);
  } else {
    return 0;
  }
}

int32_t open_fs(fs_node_t *node, uint8_t write) {
  /* If node has a open callback, call it */
  if (node->open != 0) {
    return node->open(node, write);
  } else {
    return -1;
  }
}

void close_fs(fs_node_t *node) {
  /* If node has a close callback, call it */
  if (node->close != 0)
    node->close(node);
}

uint8_t readdir_fs(fs_node_t *node, uint32_t index, fs_node_t *directory) {
  /* If the node is a directory and it has a readdir callback, call it */
  if ((node->type & LEN_7) == FS_DIRECTORY && node->readdir != 0) {
    return node->readdir(node, index, directory);
  } else {
    return 1;
  }
}

fs_node_t *finddir_fs(fs_node_t *node, char *name) {
  /* If the node is a directory and it has a finddir callback, call it */
  if ((node->type & LEN_7) == FS_DIRECTORY && node->finddir != 0) {
    return node->finddir(node, name);
  } else {
    return 0;
  }
}
