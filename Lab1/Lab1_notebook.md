> Q：[引人深思的博客](https://triplecamera.github.io/os-discussions/118)
>
> A：OS助教反馈这个问题之后会进行优化

> 往届：
>
> - 21
> 	- lab1-1
> 		- exam：输出段表内容
> 		- extra：大小端转换
> 	- lab1-2
> 		- exam：结构体输出
> 		- extra：C语言调用汇编函数
> - 22
> 	- lab1-1
> 		- exam：输出段表内容
> 		- extra：判断段表是否重叠（`ROUNDDOWN`方法计算页号）
> 	- lab1-2
> 		- exam：输出结构体
> 		- extra：C语言调用汇编函数
> - 23
> 	- exam：输出 range 格式 `(1, 2)`
> 	- extra：实现 `sprintf()` 输出到字符指针地址
> - 24
> 	- exam：定义新运算 `(x, y, |x ^ 2 - y ^ 2|)`
> 	- extra：按步骤实现 `vscanfmt`

> ==可以打开 dev 哟~~~==

> **实验代码中各个目录的组织以及其中的重要文件**
>
> <img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318160624105.png" alt="image-20250318160624105" style="zoom:50%;" />
>
> **ELF文件**
>
> <img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318170358839.png" alt="image-20250318170358839" style="zoom:50%;" />
>
> **Section Mapping**
>
> ![image-20250323211454715](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250323211454715.png)
>
> **MIPS内存布局**
>
> <img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318173608555.png" alt="image-20250318173608555" style="zoom:50%;" />
>
> <img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318173615481.png" alt="image-20250318173615481" style="zoom:50%;" />

**指针问题**

- 此处应该联想 `int a[]; a[4] == a + 4` ，当指针有了具体类型指向后，指针的数值偏移就是直接按类型大小进行

	![image-20250314164601484](C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250314164601484.png)
	
- `const char *fmt` 的定义表示 `fmt` 是一个指向常量字符的指针。这里的 `const` 修饰的是指针所指向的内容，而不是指针本身

### **MIPS**

- 涉及汇编代码编译

	```makefile
	CC := /OSLAB/compiler/usr/bin/mips_4KC-gcc
	INCLUDES := -I../include
	
	all:
		$(CC) -c my_driver.S $(INCLUDES)
		$(CC) -c my_cal.c $(INCLUDES)
	```
	
- C语言调用MIPS函数

  **使用正确的寄存器（如 `$a0`–`$a3` 用于参数传递，`$v0`–`$v1` 用于返回值）**

  ```assembly
  #include <asm/regdef.h>
  #include <asm/cp0regdef.h>
  #include <asm/asm.h>
  
  .text
  
  LEAF(_my_getchar) /*LEAF is defined in asm.h and LEAF functions don't call other functions*/
  	
  	loop_begin:
  		li s1, 0xb0000000
  		lb t1, 0x00(s1)
  		beq t1, zero, loop_begin
  		nop
  		
  	sb t1, 0x00(s1)
  	move v0, t1
  	jr ra
  	sb zero, 0x10(s1)
  	
  END(_my_getchar)            /*the function defined in asm.h*/
  
  LEAF(_my_putchar)
  
  	li s1, 0xb0000000
  	sb a0, 0x00(s1)
  	jr ra
  	sb zero, 0x10(s1)
  	
  END(_my_putchar)
  
  LEAF(_my_exit)
  
  	li s1, 0xb0000000
  	sb zero, 0x10(s1)
  	
  END(_my_exit)
  ```

- 寄存器是否需要加 `$`，建议还是看已有代码怎么写的，参照着描述

	> 某些汇编器（如 GNU 获 MARS）支持省略，但建议始终使用 `$`
	
- 如果要使用随编译工具链提供的工具 `readelf` ，用法：

	 `readelf -S <FILE>`

	`readelf -h` 查看 ELF 文件头

- `.text` 包含了可执行文件中的代码，`.data` 包含了需要 被初始化的全局变量和静态变量，而 `.bss` 包含了未初始化的全局变量和静态变量

- ~~（供快速查看）~~guidebook中，GCC 编译时指定链接方式

	`gcc -o test test.c -T test.lds -nostdlib -m32`

	```bash
	SECTIONS
	2 {
	3 . = 0x10000;
	4 .text : { *(.text) }
	5 . = 0x8000000;
	6 .data : { *(.data) }
	7 .bss : { *(.bss) }
	8 }
	```

- `EXPORT(function)` 表示声名一个函数

### **Makefile**

- `=` 递归展开赋值（变量值被使用时展开）

- `:=` 立即展开赋值（变量值在赋值时立即展开），若被展变量尚未定义，则赋为 `0`

- Makefile其他文件夹，更多采用 `$(MAKE) --directory=$targetDir clean` 格式

	`$(MAKE)` 表示当前使用的 Make 工具

	不是直接 `make` ，可以确保递归调用时使用相同的 Makefile 工具和选项
	
- 如何使用交叉编译的 `gcc` 和 `ld` 以及 `objdump`

	<img src="C:\Users\12298\AppData\Roaming\Typora\typora-user-images\image-20250318170123170.png" alt="image-20250318170123170" style="zoom:50%;" />
	
- Makefile 特殊变量

	```makefile
	$@ # 当前规则的 target
	$< # 当前规则的第一个依赖文件
	$^ # 当前规则的所有依赖文件，空格分隔
	```

### **ELF**

- ELF文件示意图

	```
	|--------------------
	|elf_header{
	|       section_header_table_offset
	|       program_header_table_offset
	|       ...
	|}
	|--------------------
	|       ...
	|--------------------
	|       section header table:
	|               an array of section header{}
	|       section header-1 {
	|               section addr
	|               ...
	|       }
	|
	|       section header-2 {
	|               section addr
	|       }
	|
	|       ...
	|--------------------
	```

### **C语言**

- 关于过程输出的测试，可以在 `init/init/c` 进行测试

- **小端 $\Leftrightarrow$ 大端**

	```c
	#define REVERSE_32(n_n) \
		((((n_n)&0xff) << 24) | (((n_n)&0xff00) << 8) | (((n_n) >> 8) & 0xff00) | (((n_n) >> 24) & 0xff))
	
	#define REVERSE_16(n_n) \
		((((n_n)&0xff) << 8) | (((n_n) >> 8) & 0xff))
	```
	
- 在大小端输出的过程中，一定要注意哪些数据需要**转换一下**才能拿出来用

  比如在 ELF 输出每个**段**或**节**的数据时，总共需要 **4** 次转换

  ```c
  // 以 0x12345678 为例
  // 大端序
  0x12 34 56 78
  // 小端序
  0x78 56 34 12
  ```

- 对于单独输出字符（不需要考虑格式时），可以用以下两种

	```c
	out(data, "(", 1);
	print_char(out, data, '(', 0, 0);
	```

- **==记得 `swtich` 的 `case` 后边要加 `break`==**

### 往届代码参考

####  **2022代码**

 `print.c`：输出结构体

  ```c
  ms = va_arg(ap, struct my_struct *);
  				
  			length = PrintChar(buf, '{', 1, 0);
  			OUTPUT(arg, buf ,length);
  			
  			int size = ms->size;
  			length = PrintNum(buf, size, 10, 0, width, ladjust, padc, 0);
  			OUTPUT(arg, buf ,length);
  			length = PrintChar(buf, ',', 1, 0);
  			OUTPUT(arg, buf ,length);
  			
  			char c = ms->c;
  			if (ladjust == 0){
  				for(i = 0; i < width - 1; i++){
  					length = PrintChar(buf, ' ', 1, 0);
  					OUTPUT(arg, buf ,length);
  				}
  			}
  			length = PrintChar(buf, c, 1, 0);
  			OUTPUT(arg, buf ,length);
  			if (ladjust == 1){
  				for(i = 0; i < width - 1; i++){
  					length = PrintChar(buf, ' ', 1, 0);
  					OUTPUT(arg, buf ,length);
  				}
  			}
  			
  			if (size == 0){
  				length = PrintChar(buf, '}', 1, 0);
  				OUTPUT(arg, buf ,length);
  			} else {
  				length = PrintChar(buf, ',', 1, 0);
  				OUTPUT(arg, buf ,length);
  			}
  			int *arr = ms->array;
  			for (i = 0; i < size; i++){
  				int temp = arr[i];
  				negFlag = 0;
  				if(temp < 0){
  					temp = -temp;
  					negFlag = 1;
  				}
  				length = PrintNum(buf, temp, 10, negFlag, width, ladjust, padc, 0);
  				OUTPUT(arg, buf ,length);
  				if (i != size - 1) {
  					length = PrintChar(buf, ',', 1, 0);
  					OUTPUT(arg, buf ,length);
  				} else {
  					length = PrintChar(buf, '}', 1, 0);
  					OUTPUT(arg, buf ,length);
  				} 
  			}
  			break;
  			
  ```

`my_driver.S`：MIPS汇编定义函数

- `kseg1` 位置在 `0xA0000000`
- 设备映射地址为 `0x10000000`
- 所以设备的实际物理地址为 `0xB0000000`

```assembly
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

.text

LEAF(_my_getchar) /*LEAF is defined in asm.h and LEAF functions don't call other functions*/
	
	loop_begin:
		li s1, 0xb0000000
		lb t1, 0x00(s1)
		beq t1, zero, loop_begin
		nop
		
	sb t1, 0x00(s1)
	move v0, t1
	jr ra
	sb zero, 0x10(s1)
	
END(_my_getchar)            /*the function defined in asm.h*/

LEAF(_my_putchar)

	li s1, 0xb0000000
	sb a0, 0x00(s1)
	jr ra
	sb zero, 0x10(s1)
	
END(_my_putchar)

LEAF(_my_exit)

	li s1, 0xb0000000
	sb zero, 0x10(s1)
	
END(_my_exit)
```

`my_cal.c`：调用汇编函数

```c
#include <my_cal.S>

char a[1086], b[1086];
int ans, i, n = 1, j;
char x;

int main()
{
	while((x = _my_getchar())!='\n')
	{
		a[i++] = x;
	}
	for (j = i - 1; j >= 0; j--)
	{
		ans += n * a[j];
		n *= 10;
	}
	for(i = 0; ans; i++)
	{
		b[i] = ans % 2 - '0';
		ans /= 2;
	}
	for (j = i - 1; j >= 0; j--)
	_my_putchar(b[j]);
}
```

`Makefile`：编译汇编代码和C语言代码

```makefile
CC := /OSLAB/compiler/usr/bin/mips_4KC-gcc
INCLUDES := -I../include

all:
	$(CC) -c my_driver.S $(INCLUDES)
	$(CC) -c my_cal.c $(INCLUDES)
```

`readelf.c`：判断段表是否重叠

其中 `ROUNDDOWN(a, n) = (((u_long)(a)) & ~((n) - 1))`

相当于是抽取了页号，页内偏移为 $\log n$ 位

```c
#define align (1LL << 12)
int readelf(u_char *binary, int size)
{
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

    int Nr;

    Elf32_Phdr *phdr1 = NULL, *phdr2 = NULL, *phdr = NULL;

    u_char *ptr_ph_table = NULL;
    Elf32_Half ph_entry_count;
    Elf32_Half ph_entry_size;

    // check whether `binary` is a ELF file.
    if (size < 4 || !is_elf_format(binary))
    {
            printf("not a standard elf format\n");
            return 0;
    }

    // get section table addr, section header number and section header size.
    ptr_ph_table = binary + ehdr->e_phoff;
    ph_entry_count = ehdr->e_phnum;
    ph_entry_size = ehdr->e_phentsize;

    // for each section header, output section number and section addr.
    // hint: section number starts at 0.
    
    int flag = 0;
    for (Nr = 1; Nr < ph_entry_count; Nr++)
    {
    	phdr1 = ((Elf32_Phdr *)(ptr_ph_table + (Nr - 1) * ph_entry_size));
    	phdr2 = ((Elf32_Phdr *)(ptr_ph_table + Nr * ph_entry_size));
    	long unsigned int r1 = phdr1->p_vaddr + phdr1->p_memsz, l2 = phdr2->p_vaddr;
    	if (ROUNDDOWN(r1, align) == ROUNDDOWN(l2, align))
		{
			printf ("Overlay at page va : 0x%x\n", ROUNDDOWN(l2, align));
			if(l2 < r1)
				printf ("Conflict at page va : 0x%x\n", ROUNDDOWN(l2, align));
			return 0;
		}
	}
    for (Nr = 0; Nr < ph_entry_count; Nr++)
	{
		phdr = ((Elf32_Phdr *)(ptr_ph_table + Nr * ph_entry_size));
    	printf("%d:0x%x,0x%x\n", Nr, phdr->p_filesz, phdr->p_memsz);
	}
    return 0;
}
```

#### **2023代码**

`printk.c`：实现 `sprintf()`

理解参数传递过程以及实现逻辑，可知不需要修改上层代码，只需要修改 `outputk` 即可

注意头文件的引用 `string.h` 以及完善 `string.c`

```c
void outputbuf(void *data, const char *buf, size_t len) {
    char *temp = (char *)data + strlen(data);
    for (int i = 0; i < len; i++) {
        *temp = buf[i];
        temp++;
    }
    *temp = '\0;'
}

int sprintf(char *buf, const char *fmt, ...) {
  char *place = buf;
  va_list ap;
  va_start(ap, fmt);
  vprintfmt(outputbuf, buf, fmt, ap);
  va_end(ap);

  return (int)(buf - place - 1);
}
```

or

```c
void outputbuf(void **data, const char *buf, size_t len) {
  for (int i = 0; i < len; i++) {
    **(char **)data = buf[i];
    (*data) = (*data) + 1;
  }
    **(char **)data = '\0';
}
// 主要不同体现在没有充当临时变量的char *temp，全程都是void **data取一层值后*data这个char *的自由移动，效果类似，不使用strlen也许效率会高一点？

// 由于outputbuf传参不同，所以这里给vprintfmt传参也需要发生改变，所以说这样的做法不太合适，相当于改变了函数结构
int sprintf(char *buf, const char *fmt, ...) {
  char *place = buf;
  va_list ap;
  va_start(ap, fmt);
  vprintfmt(outputbuf, &buf, fmt, ap);
  va_end(ap);

  return (int)(buf - place - 1);
}
```

#### **2024代码**

`print.c`

```c
case 'P':
		case 'p':
			if (long_flag) {
                                 x = va_arg(ap, long int);
                         } else {
                                 x = va_arg(ap, int);
                         }
			 tempX=x;
                         if (x < 0)
                         {
                                 if (long_flag) {
                                         x = -x;
                                 }
                                 else {
                                         x = -x;
                                 }
                                 neg_flag = 1;
 
                         }
			 temp = "(";
		       	 out(data,temp,1);
                         print_num(out, data, x, 10, neg_flag, width, ladjust, padc, 0);
			 if (long_flag) {
                                  y = va_arg(ap, long int);
                          } else {
                                  y = va_arg(ap, int);
                          }
			  tempY=y;
			  neg_flag=0;
                          if (y < 0)
                          {
                                  if (long_flag) {
                                          y = -y;
                                  }
                                  else {
                                          y = -y;
                                  }
                                  neg_flag = 1;

                          }
                          temp = ",";
                          out(data,temp,1);
                          print_num(out, data, y, 10, neg_flag, width, ladjust, padc, 0);
			  int z = (tempX + tempY) * (tempX - tempY);
			  z = z < 0 ? -z : z;
			  neg_flag=0;
			  temp = ",";
			  out(data,temp,1);
			  print_num(out,data,z,10,neg_flag,width,ladjust,padc,0);
			  temp = ")";
			  out(data,temp,1);
                         break;
```

`vscanfmt`

类似快读的方式

```c
// init.c
void scanf_1_check() {
	printk("Running scanf_1_check\n");
	char arr[128];
	char arr2[128];
	int dec, hex;
	char ch,ch2,ch3,ch4;
	scanf("%c%c%c%s%s%c",  &ch,&ch2,&ch3,arr,arr2,&ch4);
	printk("%c%c%c%s%s%c",ch,ch2,ch3,arr,arr2,ch4);
}

void mips_init(u_int argc, char **argv, char **penv, u_int ram_low_size) {
	scanf_1_check();
	halt();
}
// print.c
int vscanfmt(scan_callback_t in, void *data, const char *fmt, va_list ap) {
	int *ip;
	char *cp;
	char ch;
	int base, num, neg, ret = 0;
	int reg=0;

	while (*fmt) {
		if (*fmt == '%') {
			ret++;
			fmt++; // 跳过 '%'
			do {
				in(data, &ch, 1);
			} while (ch == ' ' || ch == '\t' || ch == '\n'); // 跳过空白符
			// 注意，此时 ch 为第一个有效输入字符
			switch (*fmt) {
			case 'd': // 十进制
				// Lab 1-Extra: Your code here. (2/5)
				neg = 0;
				if (ch == '-')
				{
					neg = 1;
					in(data, &ch, 1);
				}
				base = 10;
				num = 0;
				while(ch>='0' && ch<='9')
				{
					num = num * base;
					num = num + (ch -'0');
					in(data, &ch, 1);
				}
				ip = va_arg(ap, int*);
				if(neg == 1)
					num = -num;
				*ip = num;
				break;
			case 'x': // 十六进制
				// Lab 1-Extra: Your code here. (3/5)
				neg = 0;
				if (ch == '-' )
				{
					neg = 1;
					in(data, &ch, 1);
				}
				base = 16;
				num=0;
				while (ch>='0'&&ch<='9'||ch>='a'&&ch<='f') {
					if (ch>='a')
					{
						reg = ch-'a';
						reg+=10;
					}
					else {
						reg = ch-'0';
					}
					num=num*base;
					num=num+reg;
					in(data,&ch,1);
				}
				if (neg == 1)
					num=-num;
				ip = va_arg(ap,int*);
				*ip = num;
				break;
			case 'c':
				// Lab 1-Extra: Your code here. (4/5)
				cp = va_arg(ap,char*);
				*cp = ch;
				break;
			case 's':
				// Lab 1-Extra: Your code here. (5/5)
				cp = va_arg(ap,char*);
				num = 0;
				while(ch!='\0'&&ch!='\t'&&ch!='\n'&&ch!=' ') {
					cp[num]=ch;
					in(data,&ch,1);
					num++;		
				}
				cp[num]='\0';
				break;
			}
			fmt++;
		}
	}
	return ret;
}
```

`printk.c`：定义了输入函数，由输入函数来调用 `vscanfmt`

并且修改了头文件 `print.h` 以及 `printk.h` 以及 `machine.h`

```c
void inputk(void *data, char *buf, size_t len) {
	for (int i = 0; i < len; i++) {
		while ((buf[i] = scancharc()) == '\0') {
		}
		if (buf[i] == '\r') {
			buf[i] = '\n';
		}
	}
}

int scanf(const char *fmt, ...) {
	// Lab 1-Extra: Your code here. (1/5)
	va_list ap;
	va_start(ap, fmt);
	int result = vscanfmt(inputk, NULL, fmt, ap);
	va_end(ap);
	return result;
}
```

