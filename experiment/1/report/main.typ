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
#let lab-title = "实验1-进程控制"
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
- 加强对于进程并发执行的理解
- 实践并发进/线程的创建和控制
- 理解进程生命期期间的状态变换
- 掌握进程控制方法
= 实验过程
== 系统调用
熟悉`fork`,`exec`等系统调用
#{
    let dirname = "test/"
    let lang = "c"
    zebraw-read(dirname + "fork.c", lang)
    zebraw-read(dirname + "exec.c", lang)
    zebraw-read(dirname + "wait.c", lang)
    zebraw-read(dirname + "getpid.c", lang)
    zebraw-read(dirname + "kill_pause.c", lang)
}
== 示例程序
抄写给出的示例程序
#{
    let dirname = "example/"
    let includeDir = "include/"
    let srcDir = "src/"
    let lang = "c"
    zebraw-read(includeDir + "pctl.h", lang)
    zebraw-read(srcDir + "pctl.c", lang)
    zebraw-read(srcDir + "main.c", lang)
}
== 独立实验
根据示例程序,编写独立实验完成实验
#{
    let includeDir = "include/"
    let srcDir = "src/"
    let lang = "c"
    zebraw-read(includeDir + "pctl.h", lang)
    zebraw-read(srcDir + "pctl.c", lang)
    zebraw-read(srcDir + "main.c", lang)
}
== Build
使用xmake编译
#zebraw-read("xmake.lua", "lua")
== 实验结果
=== 示例程序
第一次运行:
```bash
xmake run example
SIGINT: Success

I am Parent process 8625
I am Child process 8627
My father is 8625
8625 Wakeup 8627 child.
8627 Process continue
8625 don't Wait for child done.

8627 child will Running:
/bin/ls -a
.  ..  example  exec  fork  getpid  kill_pause  main  wait
```
这里父进程先创建了子进程,子进程进入暂停状态等待父进程的唤醒.父进程睡眠5秒后发送SIGINT信号唤醒子进程,子进程继续执行并调用execve执行ls命令.父进程不等待子进程结束,直接继续执行并输出提示信息.

第二次运行:
```bash
make run example /bin/ls -l
SIGINT: Success

I am Parent process 8994
8994 Waiting for child done.

I am Child process 8996
My father is 8994
^Z
[1]+  Stopped                 xmake run example /bin/ls -l
ps -l
F S   UID     PID    PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
4 S  1000    6054    6052  0  80   0 -  1551 do_wai pts/3    00:00:00 bash
0 T  1000    8992    6054  1  80   0 - 22047 do_sig pts/3    00:00:00 xmake
0 T  1000    8994    8992  0  80   0 -   671 do_sig pts/3    00:00:00 example
1 T  1000    8996    8994  0  80   0 -   671 do_sig pts/3    00:00:00 example
0 R  1000    9005    6054  0  80   0 -  2079 -      pts/3    00:00:00 ps
fg
xmake run example /bin/ls -l
^C8996 Process continue
8996 child will Running:
8994 Process continue
/bin/ls -l

My child exit! status = 0


total 120
-rwxrwxrwx 1 root root 16632 Apr  3 21:16 example
-rwxrwxrwx 1 root root 16016 Apr  3 21:16 exec
-rwxrwxrwx 1 root root 16000 Apr  3 21:16 fork
-rwxrwxrwx 1 root root 16136 Apr  3 21:16 getpid
-rwxrwxrwx 1 root root 16048 Apr  3 21:16 kill_pause
-rwxrwxrwx 1 root root 16736 Apr  3 21:16 main
-rwxrwxrwx 1 root root 16040 Apr  3 21:16 wait
```
这里父进程创建了子进程并等待子进程结束.子进程进入暂停状态等待父进程的唤醒.父进程在等待状态下被用户发送了SIGINT信号,父进程和子进程都继续执行.父进程继续等待子进程结束,子进程调用execve执行ls命令.最终父进程输出子进程的退出状态.

运行独立实验:
```bash
xmake run main
=== new round ===
[child1:9840] waiting for wakeup, then run ls
[child2:9841] run ps first
    PID TTY          TIME CMD
6054 pts/3    00:00:00 bash
9836 pts/3    00:00:00 xmake
9838 pts/3    00:00:00 main
9840 pts/3    00:00:00 main
9841 pts/3    00:00:00 ps
example  exec  fork  getpid  kill_pause  main  wait
=== new round ===
[child1:9851] waiting for wakeup, then run ls
[child2:9852] run ps first
    PID TTY          TIME CMD
6054 pts/3    00:00:00 bash
9836 pts/3    00:00:00 xmake
9838 pts/3    00:00:00 main
9851 pts/3    00:00:00 main
9852 pts/3    00:00:00 ps
example  exec  fork  getpid  kill_pause  main  wait
=== new round ===
[child1:9857] waiting for wakeup, then run ls
[child2:9858] run ps first
    PID TTY          TIME CMD
6054 pts/3    00:00:00 bash
9836 pts/3    00:00:00 xmake
9838 pts/3    00:00:00 main
9857 pts/3    00:00:00 main
9858 pts/3    00:00:00 ps
example  exec  fork  getpid  kill_pause  main  wait
```
这里父进程在一个循环中不断创建两个子进程,子进程1进入暂停状态等待父进程的唤醒,子进程2执行ps命令显示当前进程状态.父进程等待子进程2结束后唤醒子进程1执行ls命令.用户可以通过发送SIGINT信号来停止父进程的循环.
= 实验总结
== 进程特征与功能
- 动态性：同一个程序经过`fork`后形成多个并发执行流,不同运行轮次对应不同进程实例.
- 并发性：父子进程、两个子进程在时间上交错推进,体现“宏观并行、微观交替”的处理机共享.
- 独立性：每个进程有独立PID、状态和返回值,父进程可等待某子进程,也可不等待继续运行.
- 异步性：执行先后并不严格固定,尤其不使用`wait/waitpid`时输出顺序可变化.
- 资源管理功能：通过`waitpid`回收子进程退出信息,避免僵尸进程；通过信号进行进程间控制.

== “进程生命期”
从代码行为可抽象出典型生命期：
- 创建：父进程执行`fork`,内核分配新的任务结构、PID和调度实体,形成子进程.
- 就绪/运行：子进程被调度后运行用户态代码,随后可能因系统调用进入内核态.
- 阻塞：调用`pause`等待信号、或父进程调用`waitpid`等待子进程时,进程进入可中断睡眠.
- 唤醒：收到`kill`发送的信号后,阻塞进程被置为可运行并再次参与调度.
- 终止：子进程`exec`失败后`_exit`,或执行的新程序结束；父进程通过`waitpid`读取并清理其退出状态.

== 进程实体与状态控制在系统中的对应
教材中的“进程实体=程序段+数据段+PCB”在实验中可对应为：
- 程序段：当前可执行映像（初始程序,或`exec`后替换成`ls/ps`映像）.
- 数据段：每个进程私有变量（例如各自的标志位、栈和全局数据副本）.
- PCB：内核维护的进程控制块,记录PID、父子关系、寄存器上下文、状态、信号掩码与退出码等.

状态控制体现在：
- `pause`触发从运行态到阻塞态.
- 信号到达触发阻塞态到就绪态/运行态.
- `waitpid`让父进程按需同步,决定“串行等待”还是“并发推进”.
- `SIGKILL/SIGINT/SIGUSR1`等不同信号体现了不同控制语义（强制终止、终端中断、用户自定义唤醒）.

== 进程与并发
- 并发不等于“同时执行每条指令”,而是多个执行流在调度器控制下有序交替.
- 仅靠创建子进程并不能保证执行次序,必须借助同步机制（`waitpid`、信号、管道等）表达先后约束.
- 并发程序的正确性核心不在“能跑起来”,而在“可控、可回收、可终止”.本实验中的异常分支清理（失败后`kill+waitpid`）很关键.

== 子进程创建与执行
- 创建阶段：`fork`复制父进程执行上下文,父子进程从同一位置继续执行,返回值不同（父得子PID,子得0）.
- 装载阶段：子进程调用`execve/execvp/execlp`后,原用户态地址空间被新程序映像替换.
- 保留关系：`exec`不改变PID与父子关系,仅替换“进程正在执行的程序内容”.
- 结束与回收：新程序退出后由父进程`waitpid`获取状态并完成回收.

== 信号机理
信号本质上是内核提供的异步事件通知机制：
- 发送：进程调用`kill`或终端产生中断,将某个信号置入目标进程的待处理集合.
- 递达：目标进程从内核态返回用户态前,内核检查未屏蔽信号并决定默认动作或进入用户注册处理函数.
- 处理：可执行默认动作（终止/忽略/停止）或自定义处理函数（如设置标志位、唤醒流程）.
- 恢复：处理完成后回到原执行流,若被信号打断阻塞系统调用,可能出现`EINTR`,程序需显式重试或清理.
