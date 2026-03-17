#import "@preview/ezexam:0.2.9": *

#show: setup.with(
    mode: EXAM,
    resume: false,
)
#show link: it => {
    set text(fill: blue)
    underline(offset: 2pt, it)
}

#title[山东大学计算机科学与技术学院 24 数据、智能操作系统课堂测验]
#notice(
    [出于方便使用#link("https://github.com/gbchu/ezexam", "gbchu/ezexam:0.2.9")作模板.],
    [源码:#link("https://github.com/Arshtyi/SDU-Operating-System").],
)
= No.1
#question()[
    列举出$5$个你了解或学习到的操作系统及它们的开发者和主要应用场景.
]
= No.2
#question()[
    给出图灵机模型的结构和计算流程.
]
= No.3
#question()[
    简述宏内核与微内核的主要区别.
]
= No.4
#question()[
    描述程序在内存中的运行时视图.
]
#question()[
    简述一个C程序代码如何变成可执行文件.
]
