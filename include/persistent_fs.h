#ifndef PERSISTENT_FS_H
#define PERSISTENT_FS_H

void pfs_init(void);
const char *pfs_pwd(void);
int pfs_cd(const char *name);
int pfs_create_dir(const char *name);
int pfs_create_file(const char *name);
int pfs_create(const char *name);
const char *pfs_read(const char *name);
int pfs_write(const char *name, const char *content);
int pfs_delete(const char *name);
void pfs_list(void (*callback)(const char *, int));

#endif
