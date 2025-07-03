# Lab2 exam / extra

> 往届上机题整理，建议就看看 23 及以后的
>
> [2024-exam](https://github.com/PhantomPower82/BUAA-OS-2024/blob/lab2-exam/kern/pmap.c)
>
> [2024-extra](https://github.com/PhantomPower82/BUAA-OS-2024/blob/lab2-extra/kern/pmap.c)
>
> [2023-exam](https://yanna-zy.github.io/2023/04/10/BUAA-OS-2/)
>
> [2023-extra](https://yanna-zy.github.io/2023/04/10/BUAA-OS-2/)
>
> [2022-1-exam](https://github.com/lklklklklklklklklklk/OS_BUAA_2022_Spring/blob/lab2-1-exam/mm/pmap.c#L72)
>
> [2022-1-extra](https://github.com/lklklklklklklklklklk/OS_BUAA_2022_Spring/blob/lab2-1-Extra/mm/pmap.c)
>
> [2022-2-exam](https://github.com/Master-Tan/BUAA_OS/blob/lab2-2-exam/mm/pmap.c)
>
> [2022-2-extra](https://github.com/Master-Tan/BUAA_OS/blob/lab2-2-Extra/mm/pmap.c)
>
> [2021-all](https://github.com/rfhits/Operating-System-BUAA-2021/blob/main/2-lab2/%E4%B8%8A%E6%9C%BA.md#lab2-1)

这次的Lab2应该是**截至目前最惨烈**的了, 主要是 extra

最后( extra ) AK 24 人, 40分 2人，其余全止步exam（相较于Lab0的AK13人，很多人拿了部分分来说，确实是最惨得了）

主播是1h40min的时候第二发过的，当时属于是求神拜佛的状态了，最后20min，应该不会有人手脚是硬的吧（

希望下次仁慈😭

> 这里有个小技巧：将自己的笔记存在跳板机的文件系统中（这里懒得放图片了，如果不知道咋处理，倒是可以加我微信hhh），通过机房的**FireFox**就可以双击下来（Chrome做不到，之前Lab1的时候室友说可以，我说真不行，后来才发现前提都不对称）。这样其实就相当于存放在了MOS的 `/tmp` 目录下。之后考试的时候就可以复制到VS Code或者Dev里边来 `Ctrl + F` 查找啦，肥肠方便~~~

> 还有就是由于我们 Lab1 的时候完成了 `printk()` 函数，调试的时候也就非常方便啦~~~

主播这次的Lab2也是准备得挺久的，notebook把几个涉及的文件中的功能函数和宏一一列出🥲，这Lab2对于我的帮助还是挺大的，最开始在寝室和室友交流的时候一直理不清这虚实的问题（结合代码和看PPT刷题真不是一个概念），后边在讨论区问了助教，然后又整理了一边，才算是理清楚，感觉挺棒的🍭

### Exam

依然是延续前两年的老传统——**遍历页表查找符合权限要求的页**，另外沿用了去年的 `[begin_va, end_va)` 的遍历区间限制，还加了一个需要对其进行 `page_remove`的要求，这里就涉及到一个在遍历查找到目标之后如何得到目标页的**虚拟地址**，其实就是 `i << 22 | j << 12`

（题目提示说整个函数按 `page_remove()` 写，因为里边有 `page_lookup()`，由于主播对于非自己写的函数并不放心，所以就还是自己写了

```c
u_int page_conditional_remove(Pde *pgdir, u_int asid, u_int perm_mask, u_long begin_va, u_long end_va) {
	u_int cnt = 0;
	u_int ld = PDX(begin_va), rd = PDX(end_va);
	u_int lp = PTX(begin_va), rp = PTX(end_va);
	for(int i = ld; i <= rd; i++) {
		u_int now = pgdir[i];
		if(!(now & PTE_V)) continue;
		Pte * pgtable = (Pte *)(KADDR(PTE_ADDR(now)));
		u_int l = (i == ld ? lp : 0), r = (i == rd ? rp : 1024);
		for(int j = l; j < r; j++) {
			u_int entry = pgtable[j];
			if(!(entry & PTE_V)) continue;
			if((PTE_FLAGS(entry) & perm_mask) != 0) {
				cnt++;
				page_remove(pgdir, asid, (i << 22) | (j << 12));
			}
		}
	}
	return cnt;
}
```

### Extra

出题的思想也是从前两年有迹可循的，大致就是内存管理——**申请 + 释放**

这次的是让管理一个用户空间的堆区域，使用的是**可变分区**

对于每个分区，有一个**元数据**，占用 24B，内容就是一个结构体，包含这个块的信息以及验证信息

- 对于 `malloc()` 就是要从给定的链表中通过**首次适应算法**找到一个满足条件的空闲块，然后按要求分给它
- 对于 `free()` 是给定一个地址，判断该位置是否可以释放，可以释放的话就从链表的**前后两项**中看**是否可以合并**

题目简述很简答，描述起来限制一大堆（（（

很多同学就是对于指针的理解不透彻（包括使用 `void *` 和 `Pte *` 不同类型指针的时机和区别）以及大大小小很多的奇葩错误，以及读题不仔细~~~

```c
#include <malloc.h>

struct MBlock_list mblock_list;

void malloc_init() {

	printk("malloc_init begin\n");

	LIST_INIT(&mblock_list);

	struct MBlock *heap_begin = (struct MBlock*) HEAP_BEGIN;

	printk("heap_begin: 0x%X\n", heap_begin);

	heap_begin->size = HEAP_SIZE - MBLOCK_SIZE;
	heap_begin->ptr = (void*) heap_begin->data;
	heap_begin->free = 1;

	LIST_INSERT_HEAD(&mblock_list, heap_begin, mb_link);

	printk("malloc_init end\n");

}

void *malloc(size_t size) {
	/* Your Code Here (1/2) */
	size = ROUND(size, 8);
	struct MBlock * block;
	LIST_FOREACH(block, &mblock_list, mb_link) {
		//
		//printk("%d %d\n", block -> size, MBLOCK_SIZE);
		//
		if((block -> size) >= size && (block -> free)) {
			size_t remain = (block -> size) - size;
			if(remain < MBLOCK_SIZE + 8) {
				size += MBLOCK_SIZE + 8;
				block -> free = 0;
				
				return (void *)(block -> data);
			} else {
				block -> free = 0;
				
				struct MBlock * now = (struct MBlock *)((block -> data) + (size));
				now -> size = (block -> size) - MBLOCK_SIZE - size;
				now -> ptr = (void *)(now -> data);
				now -> free = 1;
				// printk("now: %d\n", now -> size);
				LIST_INSERT_AFTER(block, now, mb_link);

				block -> size = size;
				// printk("%d %d\n", block -> size, now -> size);
				return (void *)(block -> data);
			}

		}
	}
	return NULL;
}

void free(void *p) {
	/* Your Code Here (2/2) */
	if((u_int)p > HEAP_BEGIN + HEAP_SIZE || (u_int)p < HEAP_BEGIN + MBLOCK_SIZE) return;
	struct MBlock * block = (struct MBlock *)(p - MBLOCK_SIZE);
	if(!(block -> ptr == block -> data)) return;
//	LIST_FOREACH(block, &mblock_list, mb_link) {
//		if((block -> ptr) == p && (block -> ptr) == (void *)(block -> data)) {
			block -> free = 1;
			if(LIST_NEXT(block, mb_link) != NULL) {
				struct MBlock * nxt = LIST_NEXT(block, mb_link);
				if(nxt -> free) {
					block -> size += nxt -> size + MBLOCK_SIZE;
					block -> free = 1;
					LIST_REMOVE(nxt, mb_link);
					(nxt -> mb_link).le_next = NULL;
					(nxt -> mb_link).le_prev = NULL;
					(nxt -> free) = 0;
					(nxt -> ptr) = NULL;

				}
			}
			
			if(LIST_FIRST(&mblock_list) != block) {
				struct MBlock * pre;
				LIST_FOREACH(pre, &mblock_list, mb_link) {
					if(LIST_NEXT(pre, mb_link) == block) {
						break;
					}
				}
				if(!(pre -> free)) return;
				pre -> size += block -> size + MBLOCK_SIZE;
				LIST_REMOVE(block, mb_link);
				(block -> mb_link).le_next = NULL;
				(block -> mb_link).le_prev = NULL;
				(block -> free) = 0;
				(block -> ptr) = NULL;
			}

	//		break;	
	//	}
	//}
}
```

`malloc.h`

```c
#ifndef _MALLOC_H_
#define _MALLOC_H_

#include <pmap.h>

#define HEAP_BEGIN 0x80400000
#define HEAP_SIZE 0x400000

#define MBLOCK_SIZE sizeof(struct MBlock)

#define MBLOCK_PREV(elm, field) (struct MBlock *)((elm)->field.le_prev) // place entry at first

LIST_HEAD(MBlock_list, MBlock);
typedef LIST_ENTRY(MBlock) MBlock_LIST_entry_t;

struct MBlock {
	MBlock_LIST_entry_t mb_link;

	u_int size;    // size of the available space, if allocated, is the size of allocated space
	void *ptr;     // pointer to the begin of block, as a magic number to check for validity
	u_int free;    // 1 if block is free, 0 if the block is allocated
	u_int padding; // padding to make size of block multiple of 8
	char data[];   // data of the block, allocated for user
};

void malloc_init(void);
void *malloc(size_t size);
void free(void *p);

#endif
```

