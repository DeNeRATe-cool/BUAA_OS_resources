# Lab1实验报告

## 思考题

### Thinking 1.1

- 对于 `objdump` 指令

	- `-D`：反汇编所有 section 的内容
	- `-S`：反汇编的时候一同输出源代码

	![image-20250318162009604](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318162009604.png)

- 实验

	- `hello.c` 文件

		![image-20250318163943968](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318163943968.png)

	- `Makefile` 文件

		![image-20250318164133305](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318164133305.png)

	- `-E` 预处理

		![image-20250318164040921](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318164040921.png)

	- `-c` 只编译不链接，之后进行目标文件反汇编

		![image-20250318165252342](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318165252342.png)

	- 编译链接后进行反汇编

		![image-20250318165600937](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318165600937.png)

	- 对两次的函数比较可以发现，函数调用的地址在链接后进行了全局指针的传递分配

		`dump_C.txt`

		![image-20250318165837381](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318165837381.png)

		`dump_EXE.txt`

		![image-20250318165815406](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318165815406.png)

### Thinking 1.2

- 使用 `readelf` 解析
	
	- 使用系统工具 `readelf` 进行解析

		![image-20250318171521647](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318171521647.png)

	- 使用我们编写的 `readelf` 尝试解析

		![image-20250318171658448](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318171658448.png)
	
- 也许你会发现我们编写的 readelf 程序是不能解析 readelf 文件本身的，而我们刚 才介绍的系统工具 readelf 则可以解析，这是为什么呢？

	- `readelf -h readelf`

		![image-20250318172146140](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318172146140.png)

	- `readelf -h hello`

		![image-20250318172449502](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318172449502.png)

	可以发现，`hello` 文件使用了 `-m32 -static -g` 编译选项，是 `ELF32` 类型的，而 `readelf` 是 `ELF64` 类型的

	说明我们的 `hello` 文件是 32 位格式，但 `readelf` 文件是 64 位的，而 `readelf.c` 文件中的所有数据类型都是基于 32 位进行设置的

	- `-m32`：编译出来的文件为 32 位程序，可以在 32 位操作系统以及 64 位操作系统运行

### Thinking 1.3

- MIPS系统启动时由 `Bootloader` 接管，通过存放的一小段代码进行硬件初始化，其运行并通过 `Linker Script` 映射形成内存映像，内核文件也在这时得知需要被加载的合适的地址

	所以内核代码会被加载到 `0x80010000` 这段地址，之后通过 `ENTRY(_start)` 便可以跳转到内核运行位置

## 实验难点

- QEMU 支持记载 ELF 格式内核，所以启动流程被简化为加载内核到内存，之后跳转到内核的入口，启动就完成了

- 内核镜像文件 mos 的 make 过程，如何进行交叉编译
	
- 实验代码中各个目录的组织以及其中的重要文件
	
	<img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318160527136.png" alt="image-20250318160527136" style="zoom:67%;" />
	
- 理解交叉编译的过程以及使用方式
	
- ELF 文件整体上分为 5 个部分

  段头表在运行时刻使用

  节头表在编译和链接时候使用

  <img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318170358839.png" alt="image-20250318170358839" style="zoom:67%;" />

- ELF 的文件头就是一个存 ELF 文件信息的结构体

- 内核通过 `VirtAddr` 指示的地址加载到正确的位置上，内核就可以运行起来

- 我们将内核放在 `kseg0` 段，运行在 `kseg1` 中的 `bootloader` 在载入内核前会进行 cache 初始化

- 程序执行的第一条指令的地址被称为入口地址（Entrypoint）

  	我们实验中的 `kernel.lds` 中通过 `ENTRY(_start)` 来设置如何为 `_start`
  

## 体会与感想

​	Lab1 并不是过一次就能够完全掌握的，我相信之后的实验也是这样。对于第一次跟着 guidebook 过一次，只能够进行简单的 Exercise 解题。但更重要的是做完之后进行不断的回顾，才能知道每一步之间的连接关系，为什么以及如何做

​	Thinking 部分我是在第二次阅读 guidebook 的时候完成的，相较于第一次，对于整体的结构有了更多的知解

​	操作系统凝结了前人的智慧，我们需要不断地一步一步理解它