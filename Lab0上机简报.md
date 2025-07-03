# Lab0 exam  / extra

> 首先是分享一个:spider_web:[学习Linux shell基础命令比较好的网站](https://www.hackerrank.com/domains/shell)，重点是可以学学 `cut` 命令，guide book 里边没怎么说（还是没说来着），但这次实验对我的帮助还是很大的。
>
> 另外就是好好使用 **tmux**，它真的超级nice！

> Extra 的题目照片附在末尾（Exam没来得及拍，提早关掉了，考试结束就没权限了）

> 补：Lab0上机的成绩出来了，100 + 50在这次测试中拿了MVP，评分13.0，原神班上只有9个人拿了 Extra 的分🥲，希望后边的实验能够仁慈一些🍭

### Exam

​	整体来说exam的知识范围还是没有超纲滴，但这个题量一上来属实有点让人开眼（T^T）[ 1 + 9 ] 超级套餐

​	第一个任务是 Makefile，唯一要注意的就是课下实验好好做，target 的依赖关系，以及非系统路径下的 include 路径设置

```makefile
all: check
	gcc -Isrc/include src/main.c src/output.c -o out/main

check: check.c
	gcc -c check.c -o check.o

run: all
	./out/main

clean:
	rm -f check.o ./out/main
```

​	第二个任务就是九连鞭，每个 `exam_{}.sh` 完成一个任务，后者依赖前者

​	前八对我来说没啥坑，涉及到的需要注意的就几个命令吧

```bash
mkdir -p [dir] # 无报错递归建文件夹

cp -r # 递归赋值文件夹

# exam_5.sh
for ((i=0;i<=20;i++))
	...
	sed "s/REPLACE/$i/g" ...
	...
	
# exam_6.sh
gcc result/code/*.c -o result/verify # gcc 通配符

# exam_7.sh
./result/verify 2>> stderr.txt # 标准输入 | 标准输出 | 标准错误

# exam_8.sh
chmod 440 stderr.txt # 权限修改的方法
```

​	第九个给我卡了快半个小时，其实就是很小的点，但给忘了，就是 `sed -n` 参数，以及 `'$1, $ p'` 之间最好加一个空格

​	最开始由于注意了后边这个点，没注意前边，没过。然后注意了前边又把后边给改回去了，真该死。最后是自己另外造数据和文件进行实验才过的，呜呜呜，卡了好久，要是没这半小时，我的 extra 说不定有希望全过😭

​	（吐槽：3min才能交一发，真的很痛苦……）

​	所以说，还是好好多注意 `sed` 的使用细节吧

```bash
# exam_9.sh
if [[ $# -eq 0 ]]
then
	cat stderr.txt
elif [[ $# -eq 1 ]]
then
	sed -n "$1,$ p" stderr.txt
else
	t=$(($2-1))
	sed -n "$1,$t p" stderr.txt
fi
```

### Extra

​	开始做 Extra 的时候只有40多分钟了，看到又是 5 个 subtask 的时候人已经碎了，只能说尽量多做了

​	但发现做得很顺畅（一发一个题），但由于最后实在是没时间了，键盘都敲出火星子了，还是没能做完，拿了 `3/5` 的 tasks，总共50分

​	主要要注意的就是如何划分文件名，题目要求划分为三部分

​	这里有些用起来很方便的命令（白天准备的时候刚好学到了一些，晚上就用上了hhh）

​	哦对，这 extra 感觉考的就是**灵活运用和搭配各个命令的能力**

```bash
# genCode.sh task 1
# 注意使用basename命令
# 以及灵活使用sed，包括其中的转义方式
# 以及find命令的通配符
...
name=$(echo $(basename $file) | cut -d '.' -f 1)
...
for file in $(find ./code -name "*.sy")
...
cat $file | sed "1i\#include\"include/libsy.h\"" | sed 's/getInt/getint/g' > codeSet/$name.c
...

# selectCode.sh task 2
# 题目给的辅助信息（软链接），但我感觉没有给好，还是自己实验几下才行
...
ln -s "codeSet/$1.c" testfile.c
...

# selectData.sh task 3
# 这个其实没啥好说的，有了 task 1 的基础，这道题很 easy 的
```

---

#### Extra题目

（顺序随机）

![_202503151737111.jpg](https://s2.loli.net/2025/03/15/Wovi6mgZhSdAlE5.jpg)

![_20250315173714.jpg](https://s2.loli.net/2025/03/15/A8W5ftH3rjOcVem.jpg)

![_202503151737131.jpg](https://s2.loli.net/2025/03/15/Rz8t9ml6xariWDu.jpg)

![_20250315173713.jpg](https://s2.loli.net/2025/03/15/xg2ScteubfBYhdU.jpg)

![_20250315173712.jpg](https://s2.loli.net/2025/03/15/qycbr21WwmM8ujx.jpg)

![_20250315173711.jpg](https://s2.loli.net/2025/03/15/srfkIVJ4Pb2A7te.jpg)