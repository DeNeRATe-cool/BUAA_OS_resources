#include <types.h>
#include <mmu.h>
#include <pmap.h>
#include <string.h>
#include <printk.h>

/*
 * 内存管理模块 - 使用首次适应算法(First-Fit)实现内存分配
 */

// 内存块控制结构体
typedef struct mem_block {
    u_int size;              // 块大小（不包括header）
    struct mem_block *next;  // 指向下一个空闲块
    int free;                // 是否空闲，1表示空闲，0表示已分配
} mem_block_t;

// 内存块头部大小
#define BLOCK_HEADER_SIZE sizeof(mem_block_t)

// 内存池起始地址
static void *mem_pool = NULL;
// 内存池大小
static u_int mem_pool_size = 0;
// 空闲块链表头
static mem_block_t *free_list = NULL;

// 内存对齐函数
static u_int align_up(u_int size, u_int align) {
    return (size + align - 1) & ~(align - 1);
}

/**
 * 初始化内存池
 * @param addr 内存池起始地址
 * @param size 内存池大小
 */
void mm_init(void *addr, u_int size) {
    mem_pool = addr;
    mem_pool_size = size;
    
    // 创建一个覆盖整个内存池的大块
    free_list = (mem_block_t *)mem_pool;
    free_list->size = size - BLOCK_HEADER_SIZE;
    free_list->next = NULL;
    free_list->free = 1;
    
    printk("Memory Manager initialized. Pool: %p, Size: %u bytes\n", mem_pool, mem_pool_size);
}

/**
 * 分配内存 - 使用首次适应算法
 * @param size 请求分配的字节数
 * @return 分配的内存地址，如果分配失败则返回NULL
 */
void *mm_malloc(u_int size) {
    // 对齐请求的大小
    u_int aligned_size = align_up(size, 4); // 按4字节对齐
    
    mem_block_t *current = free_list;
    mem_block_t *previous = NULL;
    
    // 首次适应算法：遍历空闲块链表，找到第一个足够大的块
    while (current != NULL) {
        // 只考虑空闲块
        if (current->free) {
            // 块足够大
            if (current->size >= aligned_size) {
                // 检查是否需要分割
                if (current->size >= aligned_size + BLOCK_HEADER_SIZE + 4) { // 保留最小4字节的块
                    // 分割块
                    mem_block_t *new_block = (mem_block_t *)((char *)current + BLOCK_HEADER_SIZE + aligned_size);
                    new_block->size = current->size - aligned_size - BLOCK_HEADER_SIZE;
                    new_block->next = current->next;
                    new_block->free = 1;
                    
                    current->size = aligned_size;
                    current->next = new_block;
                }
                
                // 标记为已分配
                current->free = 0;
                
                // 返回内存块数据区域地址
                return (void *)((char *)current + BLOCK_HEADER_SIZE);
            }
        }
        
        // 移动到下一个块
        previous = current;
        current = current->next;
    }
    
    // 没有找到合适的块
    printk("mm_malloc: Failed to allocate %u bytes\n", size);
    return NULL;
}

/**
 * 释放内存
 * @param ptr 要释放的内存地址
 */
void mm_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    
    // 获取块头
    mem_block_t *block = (mem_block_t *)((char *)ptr - BLOCK_HEADER_SIZE);
    
    // 确保地址在内存池范围内
    if ((void *)block < mem_pool || (void *)block >= (void *)((char *)mem_pool + mem_pool_size)) {
        printk("mm_free: Invalid pointer %p\n", ptr);
        return;
    }
    
    // 标记为空闲
    block->free = 1;
    
    // 尝试合并相邻的空闲块
    mem_block_t *current = free_list;
    
    while (current != NULL) {
        // 检查当前块是否空闲且与下一个块相邻
        if (current->free && current->next != NULL && current->next->free) {
            // 合并块
            current->size += BLOCK_HEADER_SIZE + current->next->size;
            current->next = current->next->next;
            // 继续尝试合并，不移动current指针
        } else {
            // 移动到下一个块
            current = current->next;
        }
    }
}

/**
 * 获取空闲内存大小
 * @return 总空闲内存大小（字节）
 */
u_int mm_get_free_size(void) {
    u_int free_size = 0;
    mem_block_t *current = free_list;
    
    while (current != NULL) {
        if (current->free) {
            free_size += current->size;
        }
        current = current->next;
    }
    
    return free_size;
}

/**
 * 打印内存块信息（用于调试）
 */
void mm_print_blocks(void) {
    mem_block_t *current = free_list;
    int i = 0;
    
    printk("Memory blocks:\n");
    while (current != NULL) {
        printk("[%d] Addr: %p, Size: %u, %s\n", 
               i++, current, current->size, 
               current->free ? "FREE" : "USED");
        current = current->next;
    }
    printk("Total free: %u bytes\n", mm_get_free_size());
} 