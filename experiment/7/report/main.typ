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
#let lab-title = "实验7-存储管理"
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
- 通过获取进程空间,理解Linux中进程的存储管理方式与进程地址空间的组成.
= 实验过程
== 独立实验
设计并实现:
=== 实现思路
- 入口 `src/main.cpp`, 解析 `-x/-X/pid` 参数, 默认查看当前进程.
- 数据结构定义在 `include/pm.h`, 统一描述一个虚拟内存映射区及扩展统计字段.
- 逻辑实现放在 `lib/pm.cpp`: 先读取 `/proc/<pid>/maps` 解析地址段与权限; 当使用 `-x/-X` 时, 再读取 `/proc/<pid>/smaps` 提取 `RSS/PSS/Dirty/Swap` 等字段; 最后按模式格式化输出并统计总量.
#{
    let includeDir = "include/"
    let libDir = "lib/"
    let srcDir = "src/"
    let lang = "cpp"
    zebraw-read(includeDir + "pm.h", lang)
    zebraw-read(libDir + "pm.cpp", lang)
    zebraw-read(srcDir + "main.cpp", lang)
}
== Build & Run
使用xmake构建:
#zebraw-read("xmake.lua", "lua")
测试:
#figure(image("fig/1.png"))
= 实验总结
- 本实验基于 `/proc` 文件系统复现了 `pmap` 的核心能力, 完成了基础视图与 `-x/-X` 扩展视图.
- 通过对 `maps/smaps` 的解析, 直观理解了 Linux 进程地址空间由代码段, 数据段, 堆, 栈, 共享库与匿名映射等部分组成.
- 实验结果表明: 进程虚拟内存布局与实际驻留内存 (RSS) 并不等价, 扩展字段有助于分析内存占用来源与共享情况.
