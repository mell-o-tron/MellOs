#ifdef VGA_VESA
#include "../../drivers/vesa/vesa_text.h"
#else
#include "../../drivers/vga_text.h"
#endif
#include "../../file_system/file_system.h"
#include "../../file_system/ext2/ext2.h"
#include "../../memory/dynamic_mem.h"
#include "../../memory/mem.h"
#include "../../utils/format.h"
#include "../../utils/string.h"


#define FDEF(name) void name(const char *s)

static struct ext2_inode_indexed cur_dir;

void init_fs () {
  ext2_init();
  printf("Reading current directory\n");
  cur_dir = ext2_read_inode(0xA0, 2);
  printf("num sectors in use by cur dir: %d\n", cur_dir.inode->i_sectors);
}

FDEF(ls) {
  ext2_list_files_in_dir(cur_dir.inode);
}

FDEF(cd){
  const char *t = s;

  while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
    t++;
  }

  struct ext2_inode_indexed file = find_file_by_name(cur_dir.inode, t);
			if (!file.inode) { return; }
			if (!ext2_is_directory(file.inode)) {
				printf("Selected file is not a directory.\n");
				kfree(file.inode);
			}

			printf("current directory: %s\n", t);
			cur_dir = file;
}

FDEF(newfile) {
  const char *t = s;

  while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
    t++;
  }

  if (t - s >= 128) {
    kprint("Arguments not recognized.\n");
    return;
  }

  if (strlen(t) == 0) {
    kprint("File name cannot be empty.\n");
    return;
  }

  ext2_create_inode(0xA0, EXT2_S_IFREG | EXT2_S_IRUSR | EXT2_S_IWUSR | EXT2_S_IRGRP | EXT2_S_IROTH, cur_dir, t);
}

FDEF(rmfile) {
  kprint("Not implemented\n");
  return;

  const char *t = s;

  while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
    t++;
  }

  if (t - s >= 128) {
    kprint("Arguments not recognized.\n");
    return;
  }

  // remove_file((char *)t);
}

FDEF(write_file) {
  const char *t = s;

  char filename[10];
  const char *str;

  while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
    t++;
  }

  if (t - s >= 128) {
    kprint("Arguments not recognized.\n");
    return;
  }

  uint32_t i = 0;
  while (*t != ' ' && *t != '\t' && *t != 0 && *t > 33 && *t < 126) {
    filename[i] = *t;
    t++;
    i++;
  }
  filename[i] = 0;

  // skip spaces again
  while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
    t++;
  }

  if (t - s >= 128) {
    kprint("Enter a second argument.\n");
    return;
  }

  str = t;

  // kprint("you entered the following:\n");
  // filename[9] = 0;
  // kprint(filename);
  // kprint("\n");
  // kprint(str);
  // kprint("\n");

  struct ext2_inode_indexed aa = find_file_by_name(cur_dir.inode, filename);
  ext2_write(0xA0, aa, 0, strlen(str), (uint8_t*)str);
}

FDEF(read_file) {
  const char *t = s;
  while ((*t == ' ' || *t == '\t') && (t - s < 128)) {
    t++;
  }

  struct ext2_inode_indexed file = find_file_by_name(cur_dir.inode, t);
	if (!file.inode) { return; }
	if (ext2_is_directory(file.inode)) {
		printf("Selected file is a directory.\n");
		kfree(file.inode);
	}

	char* file_block = kmalloc(file.inode->i_size + 1);
  uint8_t* res = ext2_read_from_inode(0xA0, file.inode, 0, file.inode->i_size);
  if (res) {
    memcpy(file_block, res, file.inode->i_size);
    file_block[ file.inode->i_size ] = 0; // null terminate the string
    printf("File content:\n%s\n", (char*)file_block);
  }
	kfree(file_block);
	kfree(file.inode);
}

FDEF(erase_files) {
  kprint("Not implemented - and probly never will be/n");
  return;

  prepare_disk_for_fs(3);
}
