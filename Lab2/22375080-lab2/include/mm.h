#ifndef _MM_H_
#define _MM_H_

#include <types.h>

/**
 * 初始化内存管理模块
 * @param addr 内存池起始地址
 * @param size 内存池大小（字节）
 */
void mm_init(void *addr, u_int size);

/**
 * 使用首次适应算法分配内存
 * @param size 要分配的字节数
 * @return 分配的内存地址，如果分配失败则返回NULL
 */
void *mm_malloc(u_int size);

/**
 * 释放之前分配的内存
 * @param ptr 要释放的内存地址
 */
void mm_free(void *ptr);

/**
 * 获取当前空闲内存大小
 * @return 空闲内存大小（字节）
 */
u_int mm_get_free_size(void);

/**
 * 打印内存块信息（用于调试）
 */
void mm_print_blocks(void);

#endif /* _MM_H_ */ 