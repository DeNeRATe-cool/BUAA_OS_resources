> 文件系统服务是一个**用户进程**
>
> 文件结构：
>
> - tools 目录下存放构建时辅助工具的代码，`fsformat`
> - fs 目录中存放的是文件系统服务进程的代码，`fs.c` 中实现文件系统的基本功能函数，`ide.c` 中通过系统调用与磁盘镜像进行交互，该进程的主干函数在 `serv.c` 中，通过 IPC 通信与用户进程 `user/lib/fsipc.c` 内的通信函数进行交互
> - user/lib 目录下存放用户程序的库函数，`fsipc.c` 中实现与文件系统服务进程的交互，`file.c` 中实现了文件系统的用户接口，`fd.c` 中是实现了文件描述符，允许用户程序使用统一的接口，抽象操作磁盘文件系统的文件以及控制台和管道等虚拟文件

### IDE 磁盘驱动

**访问外设**

几乎每一种外设都是通过读写设备上的寄存器来进行数据通信，外设寄存器也称为 **I/O 端口**，用来访问 I/O 设备

外设寄存器通常包括**控制寄存器、状态寄存器、数据寄存器**

寄存器被映射到指定的**物理地址**空间

由于在MOS系统中，I/O 设备的物理地址是完全固定的，因此我们可以通过简单地读写某些固定的内核虚拟地址来实现驱动程序的功能

**寻址方式**

由于按照柱面-磁头-扇区（CHS）磁盘寻址方式与该磁盘的 I/O 寄存器映射含义不完全相同，因此实验采用 **逻辑块寻址（LBA）**的方式进行扇区寻址

> 扇区编号有 **28** 位，每个扇区有 **512byte**，因此最多可寻址 **128GB** 的磁盘空间

> LBA 模式下参数和寄存器映射关系
>
> `/include/malta.h` 中存储的 PIX4 磁盘访问地址都是**物理地址**，访问时需要加上 **`0xA0000000`** 得到**虚拟地址**访问
>
> <img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250509171139568.png" alt="image-20250509171139568" style="zoom:50%;" />
>
> 寄存器对应宏如下
>
> <img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250509210322719.png" alt="image-20250509210322719" style="zoom:67%;" />

### 文件系统结构

超级块在MOS操作系统中结构如下

```c
/* Super */
struct Super {
        uint32_t s_magic;   // 魔数, 为一个常量 = FS_MAGIC
        uint32_t s_nblocks; // 记录本文件系统有多少个磁盘块, 该文件系统 1024 块
        struct File s_root; // 根目录, 其 f_type 为 FTYPE_DIR, f_name 为 '/'
};

/* File */
struct File {
        char f_name[MAXNAMELEN]; // 文件名称, 文件名最长为 MAXNAMELEN = 128
        uint32_t f_size;         // 文件大小(字节)
        uint32_t f_type;         // 文件类型, 普通文件(FTYPR_REG)和目录(FTYPE_DIR)
        uint32_t f_direct[NDIRECT]; // 文件的直接指针, 每个文件控制块设有 10 个直接指针, 记录文件的数据块在磁盘上的位置
        uint32_t f_indirect; // 文件过大时, 用来指向一个间接磁盘块, 用来存储指向文件内容的磁盘块的指针

        struct File *f_dir; // 指向文件所属的文件目录
        char f_pad[FILE_STRUCT_SIZE - MAXNAMELEN - (3 + NDIRECT) * 4 - sizeof(void *)]; // 让整数个文件结构体占用一个磁盘块, 填充结构体中剩下的字节
} __attribute__((aligned(4), packed));

/* Block ./tools/fsformat.c */
struct Block {
    uint8_t data[BLOCK_SIZE];
    uint32_t type;
} disk[NBLOCK];
```

对于普通的文件, 其指向的磁盘块存储着**文件内容**

对于目录文件, 其指向的磁盘块存储着**该目录下各个文件的文件控制块**

> 磁盘空间布局，相关数据结构定义在 `user/include/fs.h`
>
> **磁盘块有 `NBLOCK = 1024` **
>
> **磁盘块每个大小为 4KB**
>
> <img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250509210700765.png" alt="image-20250509210700765" style="zoom:50%;" />

### 文件系统的用户接口

**文件描述符**

由于建立了磁盘地址空间与进程虚拟地址空间的**缓存映射**

每打开一个文件即创建一个文件描述符代替他，一个文件描述符管理一个连续的虚拟空间，在读写时都对于这个空间进行，如有 `dirty` ，在关闭文件时重新写回磁盘进行更新

```c
struct Fd {
    u_int fd_dev_id;
    u_int fd_offset;
    u_int fd_omode;
};

struct Filefd {
    struct Fd f_fd;
    u_int f_fileid;
    struct File f_file;
}
```

文件描述符的集中地址与文件缓存区域紧邻

其中可以注意到 `PDMAP = MAXFILESIZE`，即文件的最大大小和文件在缓存区里边的最大大小一样，`INDEX2DATA` 就是使用 `PDMAP` 来通过文件描述符找到其数据的起始地址的

	// 整体的映射区间为 [FILEBASE, FILEBASE + 1024 * PDMAP)
	// 正好在存储文件描述符加文件的空间 [FILEBASE - PDMAP, FILEBASE) 的上面

## 附录

> **==文件系统概览==**
>
> ![image-20250508212553781](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250508212553781.png)

## 重要参数

```c
/* user/lib/fd.c */
#define MAXFD 32 // 文件描述符最多 32 个，即进程打开文件数量最多 32
#define FILEBASE 0x60000000 // 文件数据缓存区起始地址
#define FDTABLE (FILEBASE - PDMAP) // 存储文件描述符的起始地址

/* fsformat.c */
#define NBLOCK 1024 // 磁盘块总数

/* ./fs/serv.h */
#define SECT_SIZE 512 // 扇区大小 512B
#define SECT2BLK (BLOCK_SIZE / SECT_SIZE) // 一个磁盘块中扇区数量 8

// 缓冲区范围 0x10000000 ~ 0x4fffffff
/* Disk block n, when in memory, is mapped into the file system
 * server's address space at DISKMAP+(n*BLOCK_SIZE). */
#define DISKMAP 0x10000000
/* Maximum disk size we can handle (1GB) */
#define DISKMAX 0x40000000

/* ./usr/include/fs.h */
#define BLOCK_SIZE PAGE_SIZE // 磁盘块大小 4096B
#define BLOCK_SIZE_BIT (BLOCK_SIZE * 8) // 磁盘块位数

#define MAXNAMELEN 128 // 文件名长度 128
#define MAXPATHLEN 1024 // 路径长度 1024

#define NDIRECT 10 // 直接索引个数 10
#define NINDIRECT (BLOCK_SIZE / 4) // 一个磁盘块中索引项个数 1024

#define MAXFILESIZE (NINDIRECT * BLOCK_SIZE) // 文件最大 1024 * 4096B (直接 + 间接，间接的前边10个不用)

#define FILE_STRUCT_SIZE 256 // File结构体大小 256B

#define FILE2BLK (BLOCK_SIZE / sizeof(struct File)) // 一个磁盘块中的FCB数量
```

打开文件的模式

```c
// File open modes
#define O_RDONLY 0x0000  /* open for reading only */
#define O_WRONLY 0x0001  /* open for writing only */
#define O_RDWR 0x0002    /* open for reading and writing */
#define O_ACCMODE 0x0003 /* mask for above modes */
#define O_CREAT 0x0100   /* create if nonexistent */
#define O_TRUNC 0x0200   /* truncate to zero length */

// Unimplemented open modes
#define O_EXCL 0x0400  /* error if already exists */
#define O_MKDIR 0x0800 /* create directory, not regular file
```

## 重要函数

```c
static uint8_t wait_ide_ready();
syscall_write_dev(u_int va, u_int pa, u_int len);
syscall_read_dev(u_int va, u_int pa, u_int len);

int file_get_block(struct File *f, u_int filebno, void **blk); // 从文件中获取其数据块
int dir_alloc_file(struct File *dir, struct File **file); // 在目录中申请一个文件
int dir_lookup(struct File *dir, char *name, struct File **file); // 在目录中查找名为 name 的文件
int walk_path(char *path, struct File **pdir, struct File **pfile, char *lastelem); // 通过绝对路径查找文件

// 向文件描述符对应的数据区域进行内存读写
void *fd2data(struct Fd *fd); // 获取文件描述符的数据起始地址
int fd2num(struct Fd *fd); // 获取文件描述符的编号
static int file_write(struct Fd *fd, const void *buf, u_int n, u_int offset);
static int file_read(struct Fd *fd, void *buf, u_int n, u_int offset);
```

## 新建 `fsipc` 的过程

- ```
	user/include/fsreq.h
	```

	- 增加一个对于文件服务进程的请求类型 `FSREQ_OPENAT` 和请求结构体 `struct Fsreq_openat`；**完成基础的数据结构准备**

- ```
	user/lib/file.c
	```

	- 仿照 `open` 函数实现 `openat` 函数；**供用户程序直接启动 `openat` 流程**

- ```
	user/lib/fsipc.c
	```

	- 仿照 `fsipc_open` 实现 `fsipc_openat`；完成对 `Fsreq_openat` 各个字段的赋值；**使其能发送 `openat` 的请求**

- ```
	fs/serv.c
	```

	- 修改 `serve` 函数，使其能转发 `FSREQ_OPENAT` 请求
	- 仿照 `serve_open` 实现 `serve_openat` 函数；**实现 `openat` 整体的功能**

- ```
	fs/fs.c
	```

	- 仿照 `walk_path` 实现 `walk_path_at`；**从文件层面实现按相对路径 `path` 查找文件的功能**
	- 仿照 `file_open` 实现 `file_openat` ，类似地调用 `walk_path_at` 函数；**封装功能，供文件服务进程调用**

- 头文件：

	- `user/include/lib.h`：增加 `openat` 、 `fsipc_openat` 声明
	- `fs/serv.h`：增加 `file_openat` 声明

## 往年题

### [2023-1-exam](https://cookedbear.top/p/36639.html)

实现两个函数，来访问外设时钟

同样是使用 `syscall_read_dev() & syscall_write_dev()`

```c
u_int get_time(u_int *us) {
	u_int temp;
	temp = 0;
	syscall_write_dev(&temp, 0x15000000, 4);
	syscall_read_dev(&temp, 0x15000020, 4);
	*us = temp;
	syscall_read_dev(&temp, 0x15000010, 4);
	return temp;
}

void usleep(u_int us) {
  u_int inUs = 0;
  u_int nowUs = 0;
  u_int inTime = get_time(&inUs);

  while (1) {
    u_int nowTime = get_time(&nowUs);
    int del = (int)nowTime - (int)inTime;

    if (del * 1000000 + ((int)nowUs - (int)inUs) > us) {
      return;
    } else {
      syscall_yield();
    }
  }
}
```

### [2023-2-exam](https://cookedbear.top/p/48159.html)

一个用户接口，实现通过相对路径打开文件

> ### `user/include/fsreq.h`
>
> 没什么好说的，直接写就完了
>
> ```
> c
> #define FSREQ_OPENAT 8         // serve 函数分发时的标准
> /*-----------------------------------------------------------*/
> struct Fsreq_openat {
>     u_int dir_fileid;          // 相对的 '根目录' 打开的文件 id 
>     char req_path[MAXPATHLEN]; // 打开的文件的相对路径
>     u_int req_omode;           // 打开文件的模式
> };
> ```
>
> ### `user/lib/file.c`
>
> 供用户进程调用的 `openat` 函数
>
> ```
> c
> 
> int openat(int dirfd, const char *path, int mode) {
>   struct Fd *dir;
>   struct Fd *filefd;
>   int r;
>   /* Step 1: 获取相对目录在进程中的 fd，申请待打开的文件的 fd */
>   fd_lookup(dirfd, &dir);
>   if ((r = fd_alloc(&filefd)) != 0) { return r; }
>   
>   /* Step 2: 根据目录在文件服务进程中的 fileid，发送请求 */
>   int dir_fileid = ((struct Filefd *)dir)->f_fileid;
>   if ((r = fsipc_openat(dir_fileid, path, mode, filefd)) < 0) { return r; }
> 
>   char *va;
>   struct Filefd *ffd;
>   u_int size, fileid;
> 
>   /* Step 3: 获取到文件的 fd，对 fd 字段进行赋值，把内容 map 到 data 区域 */
>   va = fd2data(filefd);
>   ffd = (struct Filefd *)filefd;
>   size = ffd->f_file.f_size;
>   fileid = ffd->f_fileid;
> 
>   for (int i = 0; i < size; i += BY2BLK) {
>     if ((r = fsipc_map(fileid, i, (void *)(va + i))) != 0) { return r; }
>   }
>   /* Step 4: 返回文件的 fd 号 */
>   return fd2num(filefd);
> }
> ```
>
> ### `user/lib/fsipc.c`
>
> 向文件服务进程发送请求的 `fsipc_*` 函数：
>
> ```
> c
> 
> int fsipc_openat(u_int dir_fileid, const char *path, u_int omode, struct Fd *fd) {
>     struct Fsreq_openat *req;   // 注意这里的结构体类型为 openat
>     u_int perm;
>     req = (struct Fsreq_openat *)fsipcbuf;
>     if (strlen(path) >= MAXPATHLEN) {
>         return -E_BAD_PATH;
>     }
> 
>     req->req_omode = omode;
>     req->dir_fileid = dir_fileid;
>     strcpy((char *)req->req_path, path);
> 
>     return fsipc(FSREQ_OPENAT, req, fd, &perm);
> }
> ```
>
> ### `fs/serv.c`
>
> 类似于 `serve_open` 的接收请求的 `serve_openat` 函数，同时也别忘了改 `serve` 函数
>
> ```
> c
> 
> void serve_openat(u_int envid, struct Fsreq_openat *rq) {
>     struct Open *pOpen;
>     struct Open *o;
>     struct File *f;
>     struct Filefd *ff;
>     int r;
> 
>     if ((r = open_lookup(envid, rq->dir_fileid, &pOpen)) < 0) {
>         ipc_send(envid, r, 0, 0);
>         return;
>     }
>     struct File *dir = pOpen->o_file;
> 
>     if ((r = file_openat(dir, rq->req_path, &f)) < 0) {
>         ipc_send(envid, r, 0, 0);
>         return;
>     }
> 
>     if ((r = open_alloc(&o)) < 0) {
>         ipc_send(envid, r, 0, 0);
>         return;
>     }
> 
>     o->o_file = f;
> 
>     ff = (struct Filefd *)o->o_ff;
>     ff->f_file = *f;
>     ff->f_fileid = o->o_fileid;
>     o->o_mode = rq->req_omode;
>     ff->f_fd.fd_omode = o->o_mode;
>     ff->f_fd.fd_dev_id = devfile.dev_id;
> 
>     ipc_send(envid, 0, o->o_ff, PTE_D | PTE_LIBRARY);
> }
> ```
>
> ### `fs/fs.c`
>
> 底层直接执行文件打开的函数，类似于 `walk_path` & `file_open`
>
> ```
> c
> 
> int walk_path_at(struct File *par_dir, char *path, struct File **pdir,
>                  struct File **pfile, char *lastelem) {
>   char *p;
>   char name[MAXNAMELEN];
>   struct File *dir, *file;
>   int r;
> 
>   // start at the par_dir
>   file = par_dir;
>   dir = 0;
>   name[0] = 0;
> 
>   if (pdir) {
>     *pdir = 0;
>   }
> 
>   *pfile = 0;
> 
>   // find the target file by name recursively.
>   while (*path != '\0') {
>     dir = file;
>     p = path;
> 
>     while (*path != '/' && *path != '\0') { path++; }
> 
>     if (path - p >= MAXNAMELEN) { return -E_BAD_PATH; }
> 
>     memcpy(name, p, path - p);
>     name[path - p] = '\0';
>     path = skip_slash(path);
>     if (dir->f_type != FTYPE_DIR) { return -E_NOT_FOUND; }
> 
>     if ((r = dir_lookup(dir, name, &file)) < 0) {
>       if (r == -E_NOT_FOUND && *path == '\0') {
>         if (pdir) { *pdir = dir; }
> 
>         if (lastelem) { strcpy(lastelem, name); }
> 
>         *pfile = 0;
>       }
> 
>       return r;
>     }
>   }
> 
>   if (pdir) { *pdir = dir; }
> 
>   *pfile = file;
>   return 0;
> }
> 
> int file_openat(struct File *dir, char *path, struct File **file) {
>   return walk_path_at(dir, path, 0, file, 0);
> }
> ```

### [2024-exam](https://github.com/gitDebuger/2024-os-lab/blob/documents/lab5.md)

实现递归复制目录到另一个地方的函数

```c
int copy_file_content(struct File *src, struct File *dst) {
	void *src_blk, *dst_blk;
	int r;
	int nblock;
	// Calculate the total number of blocks in the source file.
	nblock = ROUND(src->f_size, BLOCK_SIZE) / BLOCK_SIZE;
	for (u_int i = 0; i < nblock; i++) {
		// Lab 5-2-Exam: Your code here. (3/6)
		// debugf("OK");
		try(file_get_block(src, i, &src_blk));
		try(file_get_block(dst, i, &dst_blk));
		// debugf("OK");
		memcpy(dst_blk, src_blk, BLOCK_SIZE);
		file_dirty(dst, i * BLOCK_SIZE);
	}
	// Flush the changes to the destination file
	file_flush(dst);
	return 0;
}

int copy_directory_contents(struct File *src, struct File *dst) {
	struct File *dir_content;
	void *blk;
	int r;
	// Iterate over each block in the source directory
	for (u_int i = 0; i < ROUND(src->f_size, BLOCK_SIZE) / BLOCK_SIZE; i++) {
		if ((r = file_get_block(src, i, &blk)) < 0) {
			return r;
		}
		dir_content = (struct File *)blk;
		for (int j = 0; j < FILE2BLK; j++) {
			if (dir_content[j].f_name[0] == '\0') continue;
			struct File *dst_file;
			// Step1: Alloc dst_file using 'dir_alloc_file'
			// Lab 5-2-Exam: Your code here. (4/6)
			try(dir_alloc_file(dst, &dst_file));
			// Step2: Assign corresponding values of 'f_name', 'f_dir', 'f_size', 'f_type' to dst_file
			strcpy(dst_file->f_name, dir_content[j].f_name);
			dst_file->f_dir = dst;
			dst_file->f_size = dir_content[j].f_size;
			dst_file->f_type = dir_content[j].f_type;

			// Step3: Invoke either 'copy_directory_contents' or 'copy_file_content',
			// depending on the value of 'f_type'.
			// Lab 5-2-Exam: Your code here. (5/6)
			if (dst_file->f_type == FTYPE_DIR) {
				try(copy_directory_contents(&dir_content[j], dst_file));
			} else if (dst_file->f_type == FTYPE_REG) {
				try(copy_file_content(&dir_content[j], dst_file));
			}
		}
	}
	file_flush(dst);
	return 0;
}

int directory_copy(char *src_path, char *dst_path) {
	struct File *src_dir, *dst_dir;
	int r;
	if ((r = file_open(src_path, &src_dir)) < 0) {
		return r;
	}
	if ((r = file_create(dst_path, &dst_dir)) < 0) {
		return r;
	}
	dst_dir->f_type = FTYPE_DIR;
	return copy_directory_contents(src_dir, dst_dir);
}
```
