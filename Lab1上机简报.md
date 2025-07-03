# Lab1 exam / extra

> 这里整理了一下往年的上机参考题，不全，但对于上机，不需要更多了
>
> [2021](https://github.com/rfhits/Operating-System-BUAA-2021/blob/main/1-lab1/%E4%B8%8A%E6%9C%BA.md)
>
> - lab1-1
> 	- exam：输出段表内容
> 	- extra：大小端转换
> - lab1-2
> 	- exam：结构体输出
> 	- extra：C语言调用汇编函数
>
> [2022](https://github.com/lklklklklklklklklklk/OS_BUAA_2022_Spring/tree/lab1-1-exam)
>
> - lab1-1
> 	- exam：输出段表内容
> 	- extra：判断段表是否重叠（`ROUNDDOWN`方法计算页号）
> - lab1-2
> 	- exam：输出结构体
> 	- extra：C语言调用汇编函数
>
> [2023](https://cookedbear.top/p/44391.html)
>
> - exam：输出 range 格式 `(1, 2)`
> - extra：实现 `sprintf()` 输出到字符指针地址
>
> [2024](https://github.com/jzygo/2024-BUAA-OS/tree/lab1-exam)
>
> - exam：定义新运算 `(x, y, |x ^ 2 - y ^ 2|)`
> - extra：按步骤实现 `vscanfmt`

本次的 Lab1 可以说是相当良心了，基本没有什么难度（只要看了往年的题目，过拟合一下就行了hhh）

最快的 AK 应该是30+min，最后满分的有110个同学（软院 + 计院）

出得很好，下次保持（

### Exam

仍然是出了三年的**定义新运算** `%k`

输出键值对 `%s => %d` ，如 `key => 666`

其中 `%s` 和 `%d` 需要满足和 `%k` 同样的输出格式

这个很OK，7min一次过

需要注意的就是熟悉 `print_str & print_num` 这些的使用原理，考场上就可以直接写了，旁边的 xd 似乎看了 guidebook 有个是个分钟才开动，这是不好的hhh

```c
		case 'k':
		case 'K':
			long v;
			char * str = (char *)va_arg(ap, char *);
			if(long_flag) {
				v = va_arg(ap, long int);
			} else {
				v = va_arg(ap, int);
			}
			print_str(out, data, str, width, ladjust);
			print_str(out, data, " => ", 0, 0);
			neg_flag = 0;
			if(v < 0) v = -v, neg_flag = 1;
			print_num(out, data, v, 10, neg_flag, width, ladjust, padc, 0);
			break;

```

### Extra

Extra纯纯数据结构题，指针指来指去

我先来吐槽一下我错在哪儿，让我写了半个小时，debug一个小时

对于一个字符指针，将其位置标记为字符串结尾 `\0`，结果我一兴奋（这是我写的最后一行代码），没有加 `*` ，damn！

`*(stream -> end) = '\0';`

要求完成一个 `fprintf()` 类似的函数，能够向缓冲区里边写东西（其实就是往一个结构体所维护的（文件）字符串里边写东西，如果看过23年的 `sprintf()` 的话，这个就是小case了

但不一样的是，你总共需要实现 5 个函数

- 文件表示方法

	```c
	typedef struct {
	    char * base;
	    char * ptr;
	    char * end;
	} FILE;
	```

- `void outputf(void *data, const char *buf, size_t len);`

	类似于 `outputk()` ，但这里的 `*data` 是有用的，具体可以好好理解一下，然后从 `buf` 里边写东西到 `data` 维护的文件里即可

	```c
	void outputf(void *data, const char *buf, size_t len) {
		FILE * f = (FILE *)data;
		for(int i = 0; i < len; i++) {
			*(f -> ptr) = buf[i];
			(f -> ptr) ++;
			if((f -> end) < (f -> ptr))
				f -> end = f -> ptr;
		}
	}
	
	```

- `FILE *fmemopen(FILE *stream, void *buf, const char *mode);`

	打开文件，并通过设置的 `mode` 进行初始化文件结构体

	模式包括 `w` 覆盖写和 `a` 追加写

	对于 `w` 覆盖写，需要初始化所有的指针为 `buf`

	对于 `a` 追加写，`base` 初始化为 `buf`，其余两个初始化为 `buf` 的第一个 `\0`，可以通过 `(char *)(buf) + strlen(buf)` 实现

	这里有个超级无敌大坑，我觉得有点挑刺了

	就是在题目描述里边给的是 `"a" & "w"`，对的，双引号，是个字符串，意思就是在测试点中存在 `mode` 指向字符串的情况，虽然合理，但是不合理！

	所以可以自己手写，或者使用 `string.c` 里边实现的 `strcmp()` 来进行处理

	当不为这两个模式的时候，返回 `NULL`

	```c
	FILE *fmemopen(FILE *stream, void *buf, const char *mode) {
		FILE * f = stream;
		if(strcmp(mode, "w") == 0) {
			f -> ptr = (char *)(buf);
			f -> base = (char *)(buf);
			f -> end = (char *)(buf);
			return stream;
		} else if(strcmp(mode, "a") == 0) {
			f -> base = (char *)(buf);
			f -> ptr = (char *)(buf) + strlen(buf);
			f -> end = (char *)(buf) + strlen(buf);
			return stream;
		} else return NULL;
	}
	```

- `int fmemprintf(FILE *stream, const char *fmt, ...);`

	与课下的 `printk()` 中的 `printk()` 类似，只是需要输出到 `stream` 空间中，所以可以直接调用写好了的 `vprintfmt` 辅助实现

	最后需要返回写入了多少个字节，用一个变量记录一下就行了，没有坑点，全靠理解

	```c
	int fmemprintf(FILE *stream, const char *fmt, ...) {
		char * begin = stream -> ptr;
		va_list ap;
		va_start(ap, fmt);
		vprintfmt(outputf, stream, fmt, ap);
		va_end(ap);
		return (int)((stream -> ptr) - begin);
	}
	```

- `int fseek(FILE *stream, long offset, int fromwhere);`

	比较新颖的，但没有技术含量（

	定义了三个宏

	```c
	#define SEEK_SET 0
	#define SEEK_CUR 1
	#define SEEK_END 2
	```

	给出 SEEK 指标（`fromwhere`）以及 `offset` ，更新 `stream` 对应的结构体文件中当且书写位置指针的位置，如果移动之后仍然在文件内，则返回 `0`，否则返回 `-1` 并且不更改指针，无坑

	```c
	int fseek(FILE *stream, long offset, int fromwhere) {
		char * a = NULL;
		if(fromwhere == SEEK_SET) {
			a = (stream -> base) + offset;
		} else if(fromwhere == SEEK_CUR) {
			a = (stream -> ptr) + offset;
		} else if(fromwhere == SEEK_END) {
			a = (stream -> end) + offset;
		} else return -1;
		if((stream -> base) <= a && a <= (stream -> end)) {
			stream -> ptr = a;
			return 0;
		} else {
			return -1;
		}
	}
	
	```

- `int fclose(FILE *stream);`

	关闭已经打开的文件，在文件的最后 `end` 处填上 `\0` 即可

	固定返回 `0`，无坑点

	```c
	int fclose(FILE *stream) {
		*(stream -> end) = '\0';
		return 0;
	}
	```

	