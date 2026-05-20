#import "@preview/ezexam:0.3.1": *
#import "@preview/subpar:0.2.2"
#import "@preview/fletcher:0.5.8" as fletcher: diagram, edge, node
#import "@preview/zero:0.6.1": num, set-num, set-unit, zi
#import "@preview/pinit:0.2.2": *

#show: setup.with(
    mode: EXAM,
    resume: false,
    heading-top: 0em,
    heading-bottom: 0.4em,
    line-height: 0.65em,
    par-spacing: 0.65em,
    enum-spacing: 0.65em,
    list-spacing: 0.65em,
)
#set par(justify: true)
#set smartquote(quotes: "\"\"")
#show link: it => text(fill: blue.darken(20%), underline(it))
#show raw: set text(font: ("IBM Plex Mono", "Noto Sans CJK SC"))
#show raw.where(block: false): box.with(
    fill: luma(240),
    inset: (x: 0.3em, y: 0em),
    outset: (x: 0em, y: 0.3em),
    radius: 0.2em,
)
#show raw.where(block: true): block.with(
    fill: luma(248),
    stroke: 0.5pt + rgb("bfbfbf"),
    inset: 0.7em,
    radius: 4pt,
)
#set-unit(fraction: "power")

#let question = question.with(supplement: "Q ", ref-on: true, show-ref-prefix: false)
#let (Hz, bit, Gbps, K, B, GB, TB) = (
    zi.declare("Hz"),
    zi.declare("bit"),
    zi.declare("Gbps"),
    zi.declare("K"),
    zi.declare("B"),
    zi.declare("GB"),
    zi.declare("TB"),
)
#let Title = "山东大学计算机科学与技术学院操作系统课堂测验"
#let author = "arshtyi"
#let date = datetime.today()

#set document(title: Title, author: author, date: date)

#title(Title)
#exam-info(info: (班级: "24智能", 教师: "刘健中"))
#notice(
    [出于方便使用#link("https://github.com/gbchu/ezexam", "gbchu/ezexam:0.3.1")作模板.],
    [源码:#link("https://github.com/arshtyi/SDU-Operating-System", "source").],
)

= No.1
#question[
    列举出$5$个你了解或学习到的操作系统及它们的开发者和主要应用场景.
]
= No.2
#question[
    给出图灵机模型的结构和计算流程.
]
= No.3
#question[
    简述宏内核与微内核的主要区别.
]
= No.4
#question[
    描述程序在内存中的运行时视图.
]
#question[
    简述一个C程序代码如何变成可执行文件.
]
= No.5
#question[
    简述直接回填法和间接地址法的优劣.
]
= No.6
#question[
    回答关于下面动态链接和静态链接的问题:
    + 给出动态链接和静态链接的程序装载过程(可用图示说明).
    + 给出动态链接和静态链接的运行时内存布局(可用图示说明).
    + 对比二者的优劣与适用场景.
]
= No.7
#question[
    简述进程和线程的异同.
]

#question[
    回答下列问题:
    + Linux中的`task`是什么?
    + 它与进程和线程的关系是什么?
]
= No.8
#question[
    简述内核线程和用户态线程(协程、纤程)的区别.
]
= No.9
#question[
    简述Linux CFS调度器的基本原理.
]
= No.10
#question[
    举例说明最佳分配算法在什么情况下会导致分配碎片.
]
= No.11
#question[
    简述可执行文件与进程的联系与区别.
]
= No.12
#question[
    简述页式内存管理中,页大小的选择需要考虑哪些因素.
]
= No.13
#question[
    简述从文件块到物理块的映射的所有中间层次及他们提供的功能.
]
= No.14
#question[
    简述混合索引法的好处以及一级索引多和三级索引多哪种性能更好.
]
= No.15
#question[
    简述ZFS相比于EXT4的优势.
]
= No.16
#question[
    证明`HDMI1.4`无法提供#K[4]\@#Hz[60]的显示输出.

    Tips:
    + `HDMI1.4`的最大带宽为#Gbps[10.2].
    + #K[4]的分辨率为$3840 times 2160$.
    + 显示器一般使用#bit[24]色彩空间.
]

= No.17
#question[
    识别并分类下面的硬件:
    + 键盘.
    + 鼠标.
    + 显示器.
    + 打印机.
    + 固态硬盘.
    + 显卡.
    + USB扩展坞.
    + 网卡.
    + 声卡.
    + 时钟芯片.
]

= No.18
#question[
    简述Peterson算法中`want`和`turn`变量各自的作用以及为什么不可省略.
]

= No.19
#question[
    简述自旋锁和阻塞锁各自的优缺点以及futex通过什么思想以达到折中.
]

= No.20
#question[
    在无锁数据结构编程中:
    + 什么是ABA问题?
    + 如何通过编程技巧避免此问题.
]

#question[
    给定一张有向图,设计算法求解是否有环.
]

= No.21
#question[
    简述资源分配图中有环是否一定等于死锁.
]
