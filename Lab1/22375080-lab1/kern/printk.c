#include <print.h>
#include <printk.h>
#include <trap.h>

/* Lab 1 Key Code "outputk" */
void outputk(void *data, const char *buf, size_t len) {
	for (int i = 0; i < len; i++) {
		printcharc(buf[i]);
	}
}
/* End of Key Code "outputk" */

/* Lab 1 Key Code "printk" */
void printk(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vprintfmt(outputk, NULL, fmt, ap);
	va_end(ap);
}
/* End of Key Code "printk" */

/* 
 * 文件系统相关定义 
 */
#define MAX_FILES 16                 /* 最大文件数 */
#define MAX_FILENAME_LEN 32          /* 最大文件名长度 */
#define FILE_BUF_SIZE 1024           /* 文件缓冲区大小 */

#define STDOUT_FD 1                  /* 标准输出文件描述符 */
#define STDERR_FD 2                  /* 标准错误文件描述符 */

typedef enum {
    FILE_TYPE_NONE,                  /* 未使用 */
    FILE_TYPE_CONSOLE,               /* 控制台设备 */
    FILE_TYPE_DISK                   /* 磁盘文件 */
} file_type_t;

/* 
 * 文件控制块结构体
 */
typedef struct {
    file_type_t type;                /* 文件类型 */
    char name[MAX_FILENAME_LEN];     /* 文件名 */
    char buffer[FILE_BUF_SIZE];      /* 文件缓冲区 */
    int buf_pos;                     /* 缓冲区当前位置 */
    int size;                        /* 文件大小 */
    int ref_count;                   /* 引用计数 */
} file_t;

/* 文件表 */
static file_t file_table[MAX_FILES];

/* 初始化文件系统 */
void fs_init(void) {
    /* 清空文件表 */
    for (int i = 0; i < MAX_FILES; i++) {
        file_table[i].type = FILE_TYPE_NONE;
        file_table[i].buf_pos = 0;
        file_table[i].size = 0;
        file_table[i].ref_count = 0;
    }

    /* 初始化标准输出和标准错误 */
    file_table[STDOUT_FD].type = FILE_TYPE_CONSOLE;
    file_table[STDOUT_FD].ref_count = 1;
    file_table[STDERR_FD].type = FILE_TYPE_CONSOLE;
    file_table[STDERR_FD].ref_count = 1;
}

/* 
 * 分配一个文件描述符
 * 返回值: 成功则返回文件描述符，失败返回-1
 */
int alloc_fd(void) {
    /* 从3开始分配（0,1,2保留） */
    for (int i = 3; i < MAX_FILES; i++) {
        if (file_table[i].type == FILE_TYPE_NONE) {
            return i;
        }
    }
    return -1;  /* 没有可用的文件描述符 */
}

/* 
 * 打开文件
 * 参数:
 *   filename - 文件名
 * 返回值: 成功则返回文件描述符，失败返回-1
 */
int open(const char *filename) {
    /* 分配文件描述符 */
    int fd = alloc_fd();
    if (fd < 0) {
        return -1;
    }

    /* 初始化文件控制块 */
    file_table[fd].type = FILE_TYPE_DISK;
    strncpy(file_table[fd].name, filename, MAX_FILENAME_LEN - 1);
    file_table[fd].name[MAX_FILENAME_LEN - 1] = '\0';
    file_table[fd].buf_pos = 0;
    file_table[fd].size = 0;
    file_table[fd].ref_count = 1;

    /* 在实际系统中，这里应该打开物理文件 */
    
    return fd;
}

/* 
 * 关闭文件
 * 参数:
 *   fd - 文件描述符
 * 返回值: 成功返回0，失败返回-1
 */
int close(int fd) {
    /* 检查文件描述符是否有效 */
    if (fd < 0 || fd >= MAX_FILES || file_table[fd].type == FILE_TYPE_NONE) {
        return -1;
    }

    /* 标准输出和标准错误不能关闭 */
    if (fd == STDOUT_FD || fd == STDERR_FD) {
        return -1;
    }

    /* 减少引用计数 */
    file_table[fd].ref_count--;
    
    /* 如果引用计数为0，释放文件描述符 */
    if (file_table[fd].ref_count == 0) {
        /* 在实际系统中，如果缓冲区中有数据，应该先刷新到磁盘 */
        if (file_table[fd].buf_pos > 0) {
            /* 刷新缓冲区到磁盘的代码 */
        }
        
        file_table[fd].type = FILE_TYPE_NONE;
    }
    
    return 0;
}

/* 
 * 写入数据到文件
 * 参数:
 *   fd - 文件描述符
 *   buf - 数据缓冲区
 *   len - 数据长度
 * 返回值: 成功写入的字节数，失败返回-1
 */
int write(int fd, const char *buf, size_t len) {
    /* 检查文件描述符是否有效 */
    if (fd < 0 || fd >= MAX_FILES || file_table[fd].type == FILE_TYPE_NONE) {
        return -1;
    }

    /* 根据文件类型处理写入操作 */
    switch (file_table[fd].type) {
        case FILE_TYPE_CONSOLE:
            /* 直接输出到控制台 */
            outputk(NULL, buf, len);
            return len;
            
        case FILE_TYPE_DISK:
            /* 将数据写入缓冲区 */
            for (size_t i = 0; i < len; i++) {
                /* 检查缓冲区是否已满 */
                if (file_table[fd].buf_pos >= FILE_BUF_SIZE) {
                    /* 在实际系统中，这里应该将缓冲区内容写入磁盘，然后清空缓冲区 */
                    printk("文件缓冲区已满，模拟刷新到磁盘\n");
                    file_table[fd].buf_pos = 0;
                }
                
                /* 写入当前字符到缓冲区 */
                file_table[fd].buffer[file_table[fd].buf_pos++] = buf[i];
                
                /* 更新文件大小 */
                if (file_table[fd].buf_pos > file_table[fd].size) {
                    file_table[fd].size = file_table[fd].buf_pos;
                }
            }
            return len;
            
        default:
            return -1;
    }
}

/* 
 * 刷新文件缓冲区
 * 参数:
 *   fd - 文件描述符
 * 返回值: 成功返回0，失败返回-1
 */
int flush(int fd) {
    /* 检查文件描述符是否有效 */
    if (fd < 0 || fd >= MAX_FILES || file_table[fd].type == FILE_TYPE_NONE) {
        return -1;
    }

    /* 只有磁盘文件需要刷新 */
    if (file_table[fd].type == FILE_TYPE_DISK && file_table[fd].buf_pos > 0) {
        /* 在实际系统中，这里应该将缓冲区内容写入磁盘 */
        printk("模拟将文件 %s 的缓冲区刷新到磁盘\n", file_table[fd].name);
        
        /* 模拟磁盘写入操作 */
        printk("写入内容: ");
        for (int i = 0; i < file_table[fd].buf_pos; i++) {
            printcharc(file_table[fd].buffer[i]);
        }
        printk("\n");
    }
    
    return 0;
}

/* 
 * file_output_t用于存储文件描述符相关信息
 */
typedef struct {
	int fd;  /* 文件描述符 */
} file_output_t;

/* 
 * 输出到文件的回调函数
 */
void output_to_file(void *data, const char *buf, size_t len) {
	file_output_t *file_data = (file_output_t *)data;
	/* 调用文件写入函数 */
	write(file_data->fd, buf, len);
}

/* 
 * fprintf: 向指定文件输出格式化字符串
 *
 * 参数:
 *   fd - 文件描述符
 *   fmt - 格式化字符串
 *   ... - 可变参数
 */
void fprintf(int fd, const char *fmt, ...) {
	file_output_t file_data;
	file_data.fd = fd;
	
	va_list ap;
	va_start(ap, fmt);
	vprintfmt(output_to_file, &file_data, fmt, ap);
	va_end(ap);
}

void print_tf(struct Trapframe *tf) {
	for (int i = 0; i < sizeof(tf->regs) / sizeof(tf->regs[0]); i++) {
		printk("$%2d = %08x\n", i, tf->regs[i]);
	}
	printk("HI  = %08x\n", tf->hi);
	printk("LO  = %08x\n\n", tf->lo);
	printk("CP0.SR    = %08x\n", tf->cp0_status);
	printk("CP0.BadV  = %08x\n", tf->cp0_badvaddr);
	printk("CP0.Cause = %08x\n", tf->cp0_cause);
	printk("CP0.EPC   = %08x\n", tf->cp0_epc);
}
