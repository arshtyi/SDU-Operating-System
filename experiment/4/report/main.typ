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
#let lab-title = "实验4-进程同步"
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
        [实验题目: #lab-title], [日期: #date], [实验课时: 2],
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
- 加强对并发协作进程同步概念的理解
- 熟练使用信号量机制解决经典同步问题
- 提升分析和设计并发程序的能力
= 实验过程
== 学习系统调用
学习以下系统调用:
- `shm`: 创建和管理共享内存
- `sem`: 创建和操作信号量
- `msg`: 进程间消息传递
#{
    let dirname = "test/"
    let lang = "c"
    zebraw-read(dirname + "shmget.c", lang)
    zebraw-read(dirname + "shmat.c", lang)
    zebraw-read(dirname + "shm_rw.c", lang)
    zebraw-read(dirname + "shmdt.c", lang)
    zebraw-read(dirname + "shmrm.c", lang)
    zebraw-read(dirname + "semget.c", lang)
    zebraw-read(dirname + "semop.c", lang)
    zebraw-read(dirname + "semctl.c", lang)
    zebraw-read(dirname + "msgget.c", lang)
    zebraw-read(dirname + "msgsnd.c", lang)
    zebraw-read(dirname + "msgrcv.c", lang)
    zebraw-read(dirname + "msgctl.c", lang)
}
== 学习示例程序
抄写并学习给出的示例程序:
#{
    let dirname = "example/"
    let includeDir = "include/"
    let libDir = "lib/"
    let srcDir = "src/"
    let lang = "c"
    zebraw-read(dirname + includeDir + "ipc.h", lang)
    zebraw-read(dirname + libDir + "ipc.c", lang)
    zebraw-read(dirname + srcDir + "producer.c", lang)
    zebraw-read(dirname + srcDir + "consumer.c", lang)
}
== 实现独立实验
设计并实现独立实验:
#{
    let includeDir = "include/"
    let libDir = "lib/"
    let srcDir = "src/"
    let lang = "c"
    zebraw-read(includeDir + "smoker.h", lang)
    zebraw-read(libDir + "smoker.c", lang)
    zebraw-read(srcDir + "main.c", lang)
    [并且实现了一个$5$生产者-$6$消费者的版本:]
    zebraw-read(srcDir + "main_many.c", lang)
}
== Build & Run
使用xmake构建:
#zebraw-read("xmake.lua", "lua")
首先进行示例实验的测试:
#figure(image("fig/1.png"))
然后进行独立实验的测试:
#figure(image("fig/2.png"))
= 实验总结
+ 真实操作系统中并发同步是通过内核调度+等待队列+原子计数实现的:
    - 用户进程调用 `semget`/`semctl`/`semop`,并不是自己改变量,而是进入内核
    - 内核对信号量进程进行调度
    - 若P后资源不足,则进程进入等待队列,直到V使条件满足唤醒
+ 对教材的适用:
    - 使用教材经典的三信号量
    - 使用前驱关系+条件同步的设计
