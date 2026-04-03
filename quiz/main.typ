// #import "@preview/ezexam:0.3.1": *
#import "../ezexam/ezexam.typ": *

#show: setup.with(
    mode: EXAM,
    resume: false,
    ref-color: black,
    // list-spacing: 1.5em,
    enum-spacing: 1.5em,
)
// #set smartquote(quotes: "\"\"")

#show link: it => text(fill: blue.darken(20%))[#underline(it)]

#title[
    山东大学计算机科学与技术学院\
    24 数据、智能操作系统课堂测验
]
#notice(
    [出于方便使用#link("https://github.com/gbchu/ezexam", "gbchu/ezexam:0.3.1")作模板.],
    [源码:#link("https://github.com/arshtyi/SDU-Operating-System").],
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
