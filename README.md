---
layout: post
title: MIT6.S081-Lab0 overview
excerpt: '本篇文章主要简单介绍下MIT6.S081这门课程，汇总一些网页资源，环境配置指南以及工具的使用。'
math: false
category_bar: true
date: 2023-08-26 15:37:51
updated:
index_img: https://cdn.jsdelivr.net/gh/tianyuxbear/blog-image/imgs/image-20230826154455432.png
categories:
- 学习笔记
- MIT6.S081
tags:
- OS
- xv6
sticky:
---

### 课程简介

MIT6.S081: Operating System Engineering 是麻省理工学院的PDOS实验室开设的面向MIT本科生的操作系统课程。PDOS的几位教授基于RISC-V开发了一个教学用操作系统xv6（一个类似于UINX v6的操作系统），课程以xv6为基础，围绕着内核、中断、系统调用、线程、上下文切换、虚拟内存、文件系统、软硬件之间的协调和交互等主题展开讲授。课程不局限于理论知识，每周都会有一个lab,让学生在xv6上增加一些新的机制和特性，帮助学生理解操作系统的众多机制和特性，十分注重学生动手能力的培养。

### 资源汇总

tip: 本人做的是2020 Fall版本的，课程不同版本之间可能略有差异。

课程网站：<https://pdos.csail.mit.edu/6.828/2020/schedule.html>

课程教材：<https://pdos.csail.mit.edu/6.828/2020/xv6/book-riscv-rev1.pdf>

课程视频（youtube）: 详见课程网站链接。

课程视频（B站：中英文字幕）：<https://www.bilibili.com/video/BV19k4y1C7kA/?spm_id_from=333.337.search-card.all.click&vd_source=df845043ad358fe9664225458a65cd42>

课程视频中文文档版：<https://mit-public-courses-cn-translatio.gitbook.io/mit6-s081/>

### 环境配置

参考：<https://pdos.csail.mit.edu/6.828/2020/tools.html>

tip: 本人实验环境：WSL2 + Ubuntu20.04

![](https://cdn.jsdelivr.net/gh/tianyuxbear/blog-image/imgs/image-20230826171103259.png) 

![](https://cdn.jsdelivr.net/gh/tianyuxbear/blog-image/imgs/image-20230826172622259.png) 

clone实验源码：

```bash
 git clone git://g.csail.mit.edu/xv6-labs-2020
 #实验根目录：你git clone 所在目录/xv6-labs-2020
 cd xv6-labs-2020
```

实验工具配置:

```bash
sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu 

# qemu-system-misc fix
# make qemu 命令可能因为qemu-system-misc版本不适配卡住，下面两条命令进行版本替换。
sudo apt-get remove qemu-system-misc
sudo apt-get install qemu-system-misc=1:4.2-3ubuntu6
```

实验根目录下`make qemu`, 终端输出`$`即证明实验环境配置成功。

![](https://cdn.jsdelivr.net/gh/tianyuxbear/blog-image/imgs/image-20230826172501620.png) 

![](https://cdn.jsdelivr.net/gh/tianyuxbear/blog-image/imgs/image-20230826172417844.png) 

xv6相关命令：

```bash
# 启动xv6===实验根目录中使用
make qemu 
# 清理上次启动生成的文件===实验根目录中使用
make clean
# 退出xv6===xv6系统中使用
Ctrl + A
# 打印进程信息===xv6系统中使用
Ctrl + P
```

### 实验导航

#### Lab1 util: Unix utilities

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/util.html>

解答：<https://codebearjourney.top/mit6.s081/lab1-util>

#### Lab2 syscall: System calls

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/syscall.html>

解答：<https://codebearjourney.top/mit6.s081/lab2-syscall>

#### Lab3 pgtbl: Page tables

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/pgtbl.html>

解答：<https://codebearjourney.top/mit6.s081/lab3-pgtbl>

#### Lab4 traps: Trap

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/traps.html>

解答：<https://codebearjourney.top/mit6.s081/lab4-traps>

#### Lab5 lazy: Lazy allocation

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/lazy.html>

解答：<https://codebearjourney.top/mit6.s081/lab5-lazy>

#### Lab6 cow: Copy-on-write fork

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/cow.html>

解答：<https://codebearjourney.top/mit6.s081/lab6-cow>

#### Lab7 thread: Multithreading

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/thread.html>

解答：<https://codebearjourney.top/mit6.s081/lab7-thread>

#### Lab8 lock: Parallelism/locking

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/lock.html>

解答：<https://codebearjourney.top/mit6.s081/lab8-lock>

#### Lab9 fs: File system

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/fs.html>

解答：<https://codebearjourney.top/mit6.s081/lab9-fs>

#### Lab10 mmap: Mmap

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/mmap.html>

解答：<https://codebearjourney.top/mit6.s081/lab10-mmap>

#### Lab11 net: Network stack

实验：<https://pdos.csail.mit.edu/6.828/2020/labs/net.html>

解答：<https://codebearjourney.top/mit6.s081/lab11-net>

### 工具使用

#### QEMU中使用GDB

参考：<https://gwzlchn.github.io/202106/6-s081-lab0/#%E5%86%99%E5%9C%A8%E5%89%8D%E9%9D%A2%E7%9A%84%E8%AF%9D>

QEMU中使用GDB进行调试时需要打开两个终端窗口(均在实验根目录下)

1. 窗口1 运行xv6
2. 窗口2 进行gdb

```bash
# 窗口1
make qemu-gdb
# 窗口2
gdb-multiarch
```

![](https://cdn.jsdelivr.net/gh/tianyuxbear/blog-image/imgs/image-20230826180413256.png)

若窗口2呈现如上图所示，则证明GDB启动成功。

如果窗口2呈现与上图不同，

```bash
# 第一次执行gdb-multiarch时先执行下这行命令
echo "add-auto-load-safe-path $(pwd)/.gdbinit " >> ~/.gdbinit
```

> tip: 上面执行完就可以对xv6 kernel进行调试了。

如何对xv6系统中单个程序进行调试？

以ls为例：

![](https://cdn.jsdelivr.net/gh/tianyuxbear/blog-image/imgs/image-20230826181759708.png) 

gdb窗口中：

```bash
# ls.c 编译为_ls, 读取_ls中的符号
file user/_ls  
# ls.c的main函数中打上断点
b main
# 继续运行
c
# 然后xv6系统会运行到`$` 处，系统启动成功。
# $ 后输入ls,回车(xv6启动窗口中)，ls并未执行，停留在main断点处。
# 让ls正常输出
c
```

