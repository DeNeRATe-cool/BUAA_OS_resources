#include <pmap.h>
#include <mm.h>
#include <printk.h>

#define MM_TEST_SIZE (64 * 1024) // 64KB用于测试

void mm_test() {
    printk("=== 首次适应内存分配算法测试 ===\n");
    
    // 为内存管理器分配内存池
    struct Page *pp;
    assert(page_alloc(&pp) == 0);
    void *pool_addr = (void *)page2kva(pp);
    
    // 初始化内存管理器
    mm_init(pool_addr, MM_TEST_SIZE);
    
    // 打印初始状态
    printk("初始内存布局:\n");
    mm_print_blocks();
    
    // 测试1: 基本分配和释放
    printk("\n测试1: 基本分配和释放\n");
    void *p1 = mm_malloc(100);
    printk("分配100字节后:\n");
    mm_print_blocks();
    
    void *p2 = mm_malloc(200);
    printk("分配200字节后:\n");
    mm_print_blocks();
    
    mm_free(p1);
    printk("释放第一个100字节后:\n");
    mm_print_blocks();
    
    void *p3 = mm_malloc(50);
    printk("分配50字节后(应该使用之前释放的100字节区域):\n");
    mm_print_blocks();
    
    mm_free(p2);
    mm_free(p3);
    printk("释放所有内存后:\n");
    mm_print_blocks();
    
    // 测试2: 内存块合并
    printk("\n测试2: 内存块合并\n");
    p1 = mm_malloc(100);
    p2 = mm_malloc(200);
    p3 = mm_malloc(300);
    void *p4 = mm_malloc(400);
    void *p5 = mm_malloc(500);
    
    printk("分配五个块后:\n");
    mm_print_blocks();
    
    // 释放几个块，测试合并
    mm_free(p2); // 释放中间的块
    printk("释放p2(200字节)后:\n");
    mm_print_blocks();
    
    mm_free(p4); // 释放另一个中间的块
    printk("释放p4(400字节)后:\n");
    mm_print_blocks();
    
    mm_free(p3); // 应该与p2和p4合并成一个大块
    printk("释放p3(300字节)后(应该与相邻块合并):\n");
    mm_print_blocks();
    
    // 测试3: 首次适应算法
    printk("\n测试3: 首次适应算法\n");
    void *p6 = mm_malloc(150);  // 应该使用前面合并的大块
    printk("分配150字节后(应该使用首个足够大的块):\n");
    mm_print_blocks();
    
    void *p7 = mm_malloc(8000); // 分配一个较大的块
    printk("分配8000字节后:\n");
    mm_print_blocks();
    
    // 测试4: 内存分配失败场景
    printk("\n测试4: 内存分配失败场景\n");
    void *p8 = mm_malloc(MM_TEST_SIZE); // 尝试分配超过剩余内存的空间
    if (p8 == NULL) {
        printk("分配失败测试通过: 无法分配超过剩余内存的空间\n");
    } else {
        printk("分配失败测试失败: 应该返回NULL但返回了 %p\n", p8);
    }
    
    // 释放所有内存
    mm_free(p1);
    mm_free(p5);
    mm_free(p6);
    mm_free(p7);
    
    printk("释放所有内存后:\n");
    mm_print_blocks();
    
    printk("=== 首次适应内存分配算法测试完成 ===\n");
}

void mips_init(u_int argc, char **argv, char **penv, u_int ram_low_size) {
    printk("init.c:\tmips_init() is called\n");

    mips_detect_memory(ram_low_size);
    mips_vm_init();
    page_init();

    mm_test();
    halt();
} 