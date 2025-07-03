# Lab4 exam / extra

> Lab4上机主要考察**系统调用以及IPC**，由于系统调用的流程非常得模板，因此看好往年题，就没啥问题，剩下的就是熟练度和C语言能力了
>
> 往年题整理如下：由于22年前没有参考意义，23 & 24年的Lab4都是分为两次的，因此虽然少，但也多（
>
> [2024-1-exam](https://github.com/gitDebuger/2024-os-lab/blob/documents/lab4.md)：添加系统调用，实现信号量机制的逻辑代码，包括信号量打开、关闭和PV操作
>
> [2024-1-extra](https://github.com/gitDebuger/2024-os-lab/blob/documents/lab4.md)：实现一个消息队列，发消息的进程往里边放信息，获取消息的进程从里边取信息
>
> [2024-2-exam](https://github.com/gitDebuger/2024-os-lab/blob/documents/lab4.md)：基于信号量机制的同步关系（用屏障来实现），实现子进程向父进程的通信
>
> [2024-2-extra](https://github.com/gitDebuger/2024-os-lab/blob/documents/lab4.md)：实现进程创建，但父子进程完全共享内存，实现 `sys_clone()` 等函数（只看代码，这题有点麻烦啊，线下看代码看不出来考场上的难度）
>
> [2023-1-exam](https://cookedbear.top/p/17374.html?highlight=lab4)（[附题目](https://yanna-zy.github.io/2023/05/15/BUAA-OS-4/)）：加了一个限制，对于同组内的进程才可以互相通信
>
> [2023-1-extra](https://cookedbear.top/p/17374.html?highlight=lab4)：仿照 `ipc_send` 函数在 `user/lib/ipc.c` 中实现 `ipc_broadcast` 函数，使得调用 `ipc_broadcast` 可以使当前进程向其后代进程（有意思，好喜欢hhh）
>
> [2023-2-exam](https://yanna-zy.github.io/2023/05/15/BUAA-OS-4/)（[附代码](https://github.com/wokron/BUAA-OS-2023/blob/lab4-2-exam/kern/syscall_all.c)）：系统调用，实现一个“屏障”（barrier）同步原语
>
> [2023-2-extra](https://github.com/wokron/BUAA-OS-2023/blob/lab4-2-extra/kern/syscall_all.c)：实现信号量机制

这次的Lab4挺简单的，让我能够第一次尝到在50min内出考场，呜呜呜，感动！！！（感谢助教怜悯！当然，希望最后一次上机也能够顺利毕业！）

竟然过的人数没有Lab1多，108人hhh，大家属于是学到现在都不想学了（总共400个人呀）

OS 马上就要结束了，现在有点得开始复习期末的感觉了（王道考研启动呜呜呜），然后我会发现，无论是学习方式还是涉及内容（原老师上课我真的很努力在听了🥹）
$$
上课 \neq 考试 \neq 课下实验 \neq 上机
$$

### Exam

分为两个部分，整体比想象中、比看起来都要简单

- part 1：增加一个系统调用，能够获取当前进程的父进程的 `env_id`

属于是走一个系统调用的过场

核心代码：

```c
int sys_get_ppid(void) {
	return curenv -> env_parent_id;
}
```



> 这里附一张系统调用流程，这位学姐写得非常细致
>
> <img src="https://s2.loli.net/2025/05/08/4cuz63NdCTvVOr5.png" alt="image-20250506200546958.png" style="zoom:50%;" />

- part 2：在 `fork` 的时候，页面映射的权限增加一个 `PTE_PROTECT` ，即被保护起来的页面，在 `fork` 时不被子进程映射

题意叙述的时候好生复杂，核心代码如下

```c
if(perm & PTE_PROTECT) return;
```

### Extra

实现一种可以被多个进程共享的页面模式

共有 `N_SHM` 个共享页面数据结构，我觉得可以称为 `SPC`

然后每个管理块中可以最多管理 `N_SHM_PAGE` 个共享页

结构体如下

```c
struct Shm {
	u_int npage;
	struct Page *pages[N_SHM_PAGE];
	u_int open;
};
```

增加 4 个系统调用实现此功能

建议使用 `page_insert() & page_decref() & page_alloc()` ，属于是把快忘完的东西又拉出来考一遍hhh

整体思路不难，重在实现的时候思路清晰，不要搞混变量了（周围很多同学都是因为搞混了，导致debug许久）

核心代码：

```c
struct Shm shm_pool[N_SHM];

int sys_shm_new(u_int npage) {
	if (npage == 0 || npage > N_SHM_PAGE) {
		return -E_SHM_INVALID;
	}

	// Lab4-Extra: Your code here. (5/8)
	int t = -1;
	for(int i = 0; i < N_SHM; i++) {
		if(shm_pool[i].open == 0) {
			t = i;
			break;
		}
	}
	if(t == -1)
		return -E_SHM_INVALID;
// page_alloc(&pp) != 0;
	for(int i = 0; i < npage; i++) {
		struct Page * pp;
		if(page_alloc(&pp) != 0) {
			for(int j = 0; j < i; j++) {
				page_decref(shm_pool[t].pages[j]);
			}
			return -E_NO_MEM;
		}
		shm_pool[t].pages[i] = pp;
		pp -> pp_ref += 1;
	}
	shm_pool[t].npage = npage;
	shm_pool[t].open = 1;
	return t;
}

int sys_shm_bind(int key, u_int va, u_int perm) {
	if (key < 0 || key >= N_SHM) {
		return -E_SHM_INVALID;
	}

	// Lab4-Extra: Your code here. (6/8)
	if(shm_pool[key].open == 0)
		return -E_SHM_NOT_OPEN;
	for(int i = va, j = 0; i < va + shm_pool[key].npage * PAGE_SIZE; i += PAGE_SIZE, j += 1) {
		try(page_insert(curenv -> env_pgdir, curenv -> env_asid, shm_pool[key].pages[j], i, perm));
	}
	return 0;
}

int sys_shm_unbind(int key, u_int va) {
	if (key < 0 || key >= N_SHM) {
		return -E_SHM_INVALID;
	}

	// Lab4-Extra: Your code here. (7/8)
	if(shm_pool[key].open == 0)
		return -E_SHM_NOT_OPEN;
	for(int i = va, j = 0; i < va + shm_pool[key].npage * PAGE_SIZE; i += PAGE_SIZE, j += 1) {
		page_remove(curenv -> env_pgdir, curenv -> env_asid, i);
	}
	return 0;
}

int sys_shm_free(int key) {
	if (key < 0 || key >= N_SHM) {
		return -E_SHM_INVALID;
	}

	// Lab4-Extra: Your code here. (8/8)
	if(shm_pool[key].open == 0) 
		return -E_SHM_NOT_OPEN;
	for(int i = 0; i < shm_pool[key].npage; i++) {
		page_decref(shm_pool[key].pages[i]);
	}
	shm_pool[key].open = 0;
	return 0;
}
```

