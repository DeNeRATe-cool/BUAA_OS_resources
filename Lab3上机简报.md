# Lab3 exam / extra

> 往届上机题目整理，同样，22年及以前没有看的必要
>
> [2024-exam](https://github.com/PhantomPower82/BUAA-OS-2024/blob/lab3-exam/kern/sched.c)：多了两个字段，`env_sched` 代表被选中上CPU的次数；`env_clocks` 代表进程实际消耗的时钟数
>
> [2024-extra](https://github.com/PhantomPower82/BUAA-OS-2024/blob/lab3-extra/kern/traps.c)：处理一些操作系统内部无法识别的指令，并实现自定义处理方式
>
> [2023-exam](https://github.com/AkashiSensei/os2023/blob/4ed9ad8da060e76d3572f314c3817f2e64c8af7b/kern/sched.c)：多了一个字段代表进程的使用用户
>
> [2023-extra](https://cookedbear.top/p/20326.html)：实现对于 `add & sub & addi` 的溢出异常处理

u1s1，Lab3 挺难的，exam > extra（对于有准备的人）

最后结果是 19 人满分，属于是一次比一次难了，助教一次比一次敢出题了🥲

主播又是 1h30+min 过的，其中接近 50min 都花到了 exam 上

这次没啥压力，最近发现 OS 没必要每次花这么多时间，只要弄懂了 guidebook ，其实本质上准备上机和复习课下实验没什么必然联系，在快到了上机的时候好好看看和做做往年题，其实就差不多了，其他时间还是花在更加有用的事儿上吧（主要是主播对于底层确实没有太大兴趣，还是算法有意思~~~）

### Exam

要实现一个 **EDF** 算法，这次的 exam 题目叙述是最长的了，给读吐了

> 插播：实验组在这次 Lab3 出好之后直接给教学组说了得好好准备 EDF，然后老师上课呢就很委婉地开始传达信息，每节课讲一次周四上机会用到 EDF，不学就不会，直到上机前最后一节课直接在课上留时间让我们自己想想如何实现hhh

对于 Env 结构体多了四个属性

```c
	LIST_ENTRY(Env) env_edf_sched_link; // 构造 env_edf_sched_list 的链表项
	u_int env_edf_runtime; // EDF 调度参数：进程在每个周期内需要运行的时间片
	u_int env_edf_period; // EDF 调度参数：进程的运行周期
	u_int env_period_deadline; // 进程当前周期的截止时间
	u_int env_runtime_left; // 进程当前周期剩余的时间片
```

题目要求完美融合 EDF 算法和 RR（时间片轮转）算法，对于这俩算法维护两个不同的链表，合力进行调度

在还有进程可以使用 EDF 的时候进行 EDF 调度，否则执行 RR，但 RR 的进程如果被 EDF 重新抢走，那么需要记录好原来进程的剩余运行 `count` ，再下次有机会运行 RR 的时候重新开始运行这个进程。这里就需要一个新的静态变量来实现记录了

（当然，这题还有很多前序工作，但都是模仿型的，不搞错就行）

```c
void schedule(int yield) {
	static int count = 0; // remaining time slices of current env
	static struct Env * lstenv = NULL;
	struct Env *e = curenv;
	static int clock = -1;
	clock += 1;

	u_int mn = 0xffffffff;
	struct Env * newenv = NULL;
	struct Env * env_i;
	u_int id = 0xffffffff;
	LIST_FOREACH(env_i, &env_edf_sched_list, env_edf_sched_link) {
		if(env_i -> env_period_deadline == clock) {
			env_i -> env_period_deadline += env_i -> env_edf_period;
			env_i -> env_runtime_left = env_i -> env_edf_runtime;
		}
		if(clock >= (env_i -> env_period_deadline - env_i -> env_edf_period) && clock < (env_i -> env_period_deadline) && env_i -> env_runtime_left != 0) {
			if(env_i -> env_period_deadline < mn || (env_i -> env_period_deadline == mn && env_i -> env_id < id)) {
				newenv = env_i;
				id = env_i -> env_id;
				mn = env_i -> env_period_deadline;
			}
		}
	}
	if(newenv != NULL) {
		e = newenv;
		e -> env_runtime_left -= 1;
		if(e -> env_runtime_left == 0) {
			e -> env_period_deadline += e -> env_edf_period;
			e -> env_runtime_left = e -> env_edf_runtime;
		}
		env_run(e);
		return;
	}

	count--;
	e = lstenv;
	if(yield || !count || e == NULL || e -> env_status != ENV_RUNNABLE) {
		if(e != NULL && e -> env_status == ENV_RUNNABLE) {
			TAILQ_REMOVE(&env_sched_list, e, env_sched_link);
			TAILQ_INSERT_TAIL(&env_sched_list, e, env_sched_link);
		}
		if(TAILQ_EMPTY(&env_sched_list)) {
			panic("schedule: no runnable envs");
		}
		e = TAILQ_FIRST(&env_sched_list);
		count = e -> env_pri;
		lstenv = e;
	}
	env_run(e);
}

```

### Extra

要实现一个对于 `lw` 和 `sw` 访存时出现未按**字**对齐的中断的处理，例如对于访问 `tf -> regs[base] + offset = 0x1`，那么需要改为访问 `0x0`，即向下取整

但题目限制只能通过修改 `offset` 来进行完善，因此我们只需要记录访存地址相较于对齐地址的差，对 `offset` 减去即可，另外由于 `offset` 可能为负数，且其为 16 位数（即 `short`），需要进行一些处理放置负数出错

最后对于 `lw` 修改寄存器真值，对于 `sw` 修改内存中真值即可，然后 `tf -> epc += 4` 访问下一条指令

哦对，还有就是通过用户态的中断地址访问指令，由于那段地址是**只读**的，所以需要转换为内核态 `KADDR` 进行访问（要是看过往年题，这部分很简单，不然，真得理解理解）

> 好，题目到这里我觉得我叙述还是非常清晰的
>
> 但是，考场上的这道题可很不清晰，属于是边试着理解题意边提交，十分折磨
>
> 题目的样例也没有说清楚最后输出是什么，因为按照正解来做也会有 `panic` ，但其他的结果是对的，甚至是找不到 `schedule env`，hhh

实现步骤也有一些前戏，比如补全 `kern/genex.S` 和 `kern/traps.c`，这些都是小问题

（以下是我的超级繁琐的代码，仅供参考）

```c

void do_adel(struct Trapframe *tf) { // 处理 lw
	u_long va = tf -> cp0_epc;
	Pte * pte;
	page_lookup(curenv -> env_pgdir, va, &pte);
	u_long pa = PTE_ADDR(*pte) | (va & 0xfff);
	u_long kva = KADDR(pa);
	u_int * instr = (u_int *) kva;
	u_int base = ((*instr) >> 21) & 0x1F;
	u_int rt = ((*instr) >> 16) & 0x1F;
	u_int imm = (*instr) & 0xffff;
	u_int immm = imm;
	if(imm & 0x8000) immm |= 0xFFFF0000;
	u_int loc = tf -> regs[base] + immm;
	// printk("base = %u rt = %u immm = %u loc = %u\n", base, rt, immm, loc);
	u_int offset = loc & 0x3;
	// printk("offset = %u\n", offset);
	u_int ans = imm - offset;
	// u_int ans = ((short)(imm) - (short)(offset)) & 0xFFFF;
	u_int new_inst = ((*instr) & (~0xffff)) | ans;
	*instr = new_inst;
	printk("AdEL handled, new imm is : %04x\n", new_inst & 0xffff); // 这里的 new_inst 替换为修改后的指令
	tf -> regs[rt] = (*((u_int *)(loc & (~0x3))));
	tf->cp0_epc += 4;
}

void do_ades(struct Trapframe *tf) { // 处理 sw
 	u_long va = tf -> cp0_epc;
	Pte * pte;
	page_lookup(curenv -> env_pgdir, va, &pte);
	u_long pa = PTE_ADDR(*pte) | (va & 0xfff);
	u_long kva = KADDR(pa);
	u_int * instr = (u_int *) kva;
	u_int base = ((*instr) >> 21) & 0x1F;
	u_int rt = ((*instr) >> 16) & 0x1F;
	u_int imm = (*instr) & 0xffff;
	u_int immm = imm;
	if(imm & 0x8000) immm |= 0xFFFF0000;
	u_int loc = tf -> regs[base] + immm;
	// printk("base = %u rt = %u immm = %u loc = %u\n", base, rt, immm, loc);
	u_int offset = loc & 0x3;
	// printk("offset = %u\n", offset);
	u_int ans = imm - offset;
	// u_int ans = ((short)(imm) - (short)(offset)) & 0xFFFF;
	u_int new_inst = ((*instr) & (~0xffff)) | ans;
	*instr = new_inst;
	printk("AdES handled, new imm is : %04x\n", new_inst & 0xffff); // 这里的 new_inst 替换为修改后的指令
	*((u_int *)(loc & (~0x3))) = tf -> regs[rt];
	tf->cp0_epc += 4;	// 在此实现相应操作以使修改后指令符合要求
}
```

