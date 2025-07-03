# Lab3实验报告

### 思考题

#### Thinking 3.1

- `UVPT` 表示用户页表的起始虚拟地址
- `PDX(UVPT)` 表示取其页目录索引号
- `e -> env_pgdir[PDX(UVPT)]` 表示当前进程内存中 `UVPT` 地址的页目录项
- `PADDR(e -> env_pgdir)` 表示当前进程页目录的物理地址
- 整体来说就是当前进程中 `UVPT` 地址的页目录项赋值为自己的页目录的物理基地址，加上去权限

#### Thinking 3.2

`env.c`

![image-20250415203144669](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250415203144669.png)

![image-20250415203236637](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250415203236637.png)

其中，`data` 来源于 `load_icode` 中通过函数传递的进程控制块指针 `e`

这个参数必须有，不然 `load_icode_mapper` 就无法通过 `page_insert` 将文件插入页表

#### Thinking 3.3

- 修改权限位
- 虚拟地址页面对齐
- 将段内页映射导物理空间
- 把多余的空间用 0 填充

![image-20250415203706084](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250415203706084.png)

#### Thinking 3.4

- 虚拟地址

因为程序计数器始终指向虚拟地址，由MMU来负责将其转换为物理地址进行访问，若 `cp0_epc` 存储物理地址，CPU需关闭MMU直接访问物理地址

#### Thinking 3.5

`kern/traps.c`

![image-20250415204809838](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250415204809838.png)

`kern/genex.S`

![image-20250415205012966](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250415205012966.png)

#### Thinking 3.6

`kern/entry.S`进入异常处理

![image-20250415205412360](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250415205412360.png)

`kern/env_asm.S` 重置时钟

![image-20250415205327830](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250415205327830.png)

`kern/genex.S` 打开中断

![image-20250415205224557](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250415205224557.png)

#### Thinking 3.7

中断后，系统将PC指向 `0x80000180` ，跳转到 `exc_gen_entry` 代码段，之后进行异常分发 `handle_int`，之后进入 `schedule` 进行进程调度

在 `schedule` 函数中

先获取当前进程的进程控制块，将其可用全局静态变量时间片减一，判断是否达到切换进程的情况，如果当前有进程并话可以运行，则将其加入调度队列尾部，之后从队头取出一个进程，设置优先级等，然后进行切换进程，如果未达到调度情况，则不切换

### 实验难点

- 主要集中在对于中断的理解，中断流程以及中断前后进程以及内核以及寄存器发生了什么变化，需要多多参悟
- 对于寄存器恢复的理解，`Env` 块中的 `TrapFrame` 的恢复以及保存
- 另外就是连接内核空间与用户空间的桥梁打通了，需要将这两者好好区分

### 体会与感想

随着MOS操作系统越来越完善，系统的复杂度也是陡增，大量的函数调用以及宏的使用，让同学们对于代码的理解难度提升。这也是我希望能够多花些时间在Lab上进行更多思考的原因，在实验前后好好温习，上机的时候以及课后才能深入理解操作系统的核心！