#import "@preview/codly:1.3.0": *
#import "@preview/cetz:0.4.2"
#import "@preview/numbly:0.1.0": numbly
#import "@preview/zebraw:0.6.1": *

// Define some variables for the report
#let institute = "计算机科学与技术"
#let course = "操作系统"
#let student-id = "202400130242"
#let student-name = "彭靖轩"
#let date = datetime.today().display("[year].[month].[day]")
#let lab-title = "实验2-线程和管道通信"
#let class = "24智能"
#let font = (
    main: "IBM Plex Serif",
    mono: "Fira Code",
    cjk: "Noto Serif CJK SC",
    math: "New Computer Modern Math",
)

// Color palette
#let palette = (
    link: rgb("#1D4F91"),
    ref: rgb("#6A3FB5"),
    highlight: (
        rgb("#DCEEFF"), // blue
        rgb("#DFF3F0"), // teal
        rgb("#ECE7FF"), // violet
        rgb("#EAF4D8"), // olive
    ),
)

// Set up styles
#set document(title: lab-title, author: student-name)
#set text(
    font: (font.main, font.cjk),
    size: 11pt,
    lang: "en",
    region: "us",
)
#set smartquote(quotes: "\"\"")
#set page(
    paper: "a4",
    margin: (x: 25pt, y: 25pt),
    footer: context {
        set align(center)
        set text(9pt)
        counter(page).display("1 / 1", both: true)
    },
    header: counter(footnote).update(0),
)
#set heading(
    numbering: numbly(
        "{1:1}",
        "{2:1}.",
        "({3:1})",
    ),
)
#show heading.where(level: 1): it => block(above: .6em, it.body)
#show heading: it => if (it.level == 1) {
    set align(center)
    show h.where(amount: .3em): none
    text(size: 15pt, it)
} else {
    text(size: 13pt, it)
}
#show heading.where(level: 1): set heading(supplement: [实验])
#set par(justify: true, first-line-indent: (amount: 2em, all: true))
#show raw: set text(font: ((name: font.mono, covers: "latin-in-cjk"), font.cjk))
#show link: it => text(fill: palette.link, style: "italic", underline(evade: false, it))
#show ref: set text(fill: palette.ref)
#let cite = cite.with(style: "ieee")
#set footnote(numbering: "[1]")
#set list(indent: 6pt, marker: sym.bullet.tri)
#set enum(indent: 6pt, numbering: numbly(n => emph(strong(numbering("1.", n)))))

#{
    show heading: it => align(center)[#text(size: 18pt, tracking: 0.1em, weight: "bold", it)]
    heading(
        numbering: none,
        level: 1,
        bookmarked: false,
        outlined: false,
    )[#institute 学院 #underline(offset: 4pt, extent: 6pt, [#course]) 课程实验报告]
    set text(size: 12pt)
    set table.cell(align: left + horizon, inset: 6pt)
    table(
        columns: (1fr,) * 3,
        [学号: #student-id], [姓名: #student-name], [班级: #class],
        [题目: #lab-title], [日期: #date], [课时: 2],
    )
}

// Let line numbers be more visible and add highlight for changed lines
#let zebraw = zebraw.with(
    numbering-separator: true,
    radius: 10pt,
    lang: false,
    highlight-color: palette.highlight,
)
#let zebraw-read(filepath, lang) = {
    zebraw(header: filepath, raw(read("../" + filepath), block: true, lang: lang))
}

= 实验目的
- 熟悉`pthread`库的使用
- 练习基于`pthread`库、利用无名管道进行线程通信的编程和调试技术
= 实验过程
== 学习使用库
熟悉`pthread`库的使用
#{
    let dirname = "test/"
    let lang = "c"
    zebraw-read(dirname + "pthread.c", lang)
    zebraw-read(dirname + "join_exit.c", lang)
    zebraw-read(dirname + "detach_cancel.c", lang)
}
== 示例程序
抄写给出的示例程序
#{
    let dirname = "example/"
    let srcDir = "src/"
    let lang = "c"
    zebraw-read(dirname + srcDir + "tpipe.c", lang)
    zebraw-read(dirname + srcDir + "ppipe.c", lang)
}
== 独立实验
根据示例程序,编写独立实验完成实验
#{
    let dirname = "src/"
    let lang = "c"
    zebraw-read(dirname + "main.c", lang)
}
== Build
使用`xmake`构建示例程序和独立实验程序
#zebraw-read("xmake.lua", "lua")
== 实验结果
=== 示例程序
首先运行示例程序`tpipe`:
```bash
xmake run tpipe
thread2 read: 1
thread1 read: 2
thread2 read: 3
thread1 read: 4
thread2 read: 5
thread1 read: 6
thread2 read: 7
thread1 read: 8
thread2 read: 9
thread1 read: 10
```
这个示例程序中,两个线程通过两个无名管道进行通信,合作将整数从1递增到10

接着运行示例程序`ppipe`:
```bash
xmake run ppipe
child 685 read: 1
parent 683 read: 2
child 685 read: 3
parent 683 read: 4
child 685 read: 5
parent 683 read: 6
child 685 read: 7
parent 683 read: 8
child 685 read: 9
parent 683 read: 10
```
该示例程序中并发的父子进程通过两个无名管道进行通信,合作将整数从1递增到10
=== 独立实验
运行独立实验程序`main`:
```bash
xmake run main
Enter x, y: 10 10
thread f(x): f(10) = 3628800
thread f(y): f(10) = 55
thread f(x,y): f(x,y) = f(x) + f(y) = 3628800 + 55 = 3628855
```
基于无名管道的三个并发协作线程分别计算了$f(x),f(y),f(x,y)$的结果.
= 实验总结
== 进/线程协作与通信概念
本次实验中的`pthread`创建、`join`/`detach`/`cancel`以及`ppipe`和`tpipe`示例,完整体现了教材里“并发执行+协作同步+数据通信”的核心结构.

从现象上看,不论是父子进程还是两个线程,都不是各自独立完成任务,而是按照“写入一方-读取一方-反馈一方”的次序推进,这说明通信机制不仅传递数据,也在约束执行顺序,从而实现协作.

== 实现方式
在真实操作系统中,进程与线程的通信和协作由内核对象与调度机制共同完成:
- 进程彼此地址空间隔离,通常通过IPC(管道、消息队列、共享内存等)通信;
- 线程共享同一进程地址空间,可直接共享数据,也可通过文件描述符机制(如管道)通信;
- `read`在无数据时阻塞,`write`在缓冲区满时阻塞,内核通过等待队列与唤醒机制保障同步;
- `close`会影响管道端点引用计数,决定对端何时读到EOF,这直接关系到程序能否正确结束.

因此,教材中“进/线程通信”在系统层面并不是抽象口号,而是明确对应到系统调用语义、调度状态切换和内核缓冲区管理.

== 进/线程协作和通信
通过实验我形成了三点新的认识:
- 通信机制本身就是一种同步机制,不仅“传值”,还“控序”;
- 协作正确性优先于局部性能,必须先保证不会死锁、不会永久阻塞;
- 资源回收和端点关闭是并发程序正确性的组成部分,不是可有可无的收尾步骤.

== 管道机制机理与使用
管道本质是内核中的一段有界字节流缓冲区,通过一组读写文件描述符暴露给用户程序.
单根无名管道通常是半双工,双向通信一般使用两根管道.

利用管道完成进/线程协作与通信时,应遵循以下步骤:
- 先设计通信方向和协议(谁先写、谁先读、何时结束);
- 及时关闭不用的端点,避免“对端一直等不到结束信号”的假活跃;
- 对读写阻塞行为有预期,必要时结合非阻塞I/O或多路复用;
- 在主控执行流中正确回收资源(线程`join`/`detach`,进程`wait`).

本实验中`ppipe`与`tpipe`都采用“两根管道+交替读写”的方式,清晰展示了如何把并发执行流组织为可控、可终止、可验证的协作通信过程.
