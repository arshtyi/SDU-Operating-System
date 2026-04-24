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
#let lab-title = "实验5-进程互斥"
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
- 进一步研究并发进程同步于互斥的机制
- 加深关于非对称性互斥问题的理解
- 观察并学习非对称性互斥问题的解决方案
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
    zebraw-read(dirname + srcDir + "control.c", lang)
    zebraw-read(dirname + srcDir + "reader.c", lang)
    zebraw-read(dirname + srcDir + "writer.c", lang)
}
== 实现独立实验
设计并实现独立实验:
#{
    let includeDir = "include/"
    let libDir = "lib/"
    let srcDir = "src/"
    let lang = "c"
    zebraw-read(includeDir + "ipc.h", lang)
    zebraw-read(libDir + "ipc.c", lang)
    zebraw-read(srcDir + "main.c", lang)
}
== Build & Run
使用xmake构建:
#zebraw-read("xmake.lua", "lua")
首先进行示例实验的测试:
#figure(image("fig/1.png"))
然后进行独立实验的测试:
#figure(image("fig/2.png"))
= 实验总结
- 对 "非对称互斥算法" 的新认识:
    + 算法可以保证同一时刻只有一个进程进入临界区, 但不能自然保证两个进程轮流进入.
    + 互斥性(safety)只回答 "会不会同时进入", 而公平性和有界等待(liveness)回答 "是否每个进程都能在有限时间内进入".
    + 非对称设计如果带有固定偏置, 即使不死锁, 也可能长期不公平.
- 为什么会出现进程饥饿:
    + 出现饥饿的根因是同步条件与调度时序叠加后, 某些进程长期得不到资源.
    + 原因包括固定优先级, 自旋等待缺少排队约束, 条件判断顺序偏置, 以及快进程反复重入临界区.
- 本实验的饥饿现象表现:
    + 一个进程多次打印进入临界区和完成操作的信息, 另一个进程长时间停留在等待或重试状态.
    + 程序整体并未卡死, 但被饿死的进程在较长时间窗口内几乎没有有效进展.
- 如何解决并发进程间的饥饿:
    + 在互斥算法中加入有界等待机制, 例如 Peterson 思想, Bakery 号牌机制, 或 FIFO 队列锁.
    + 在实现层面采用公平锁策略, 避免单个进程长期占优.
    + 配合缩短临界区和合理让出 CPU, 减少快进程持续重入造成的压制.
- 对消息传递解决进程通信问题的新认识:
    + 消息传递把共享变量竞争问题转化为消息协议问题, 竞争面更可控.
    + 发送与接收的阻塞语义可以直接表达同步关系, 通信边界也更清晰.
    + 这类方案能降低共享内存条件下的数据竞争风险, 但需要额外处理消息格式, 顺序, 超时和重发等协议细节.
