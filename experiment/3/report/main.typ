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
#let lab-title = "实验3-进程综合实验"
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
- 掌握操作系统shell 的工作机制与实现过程
- 练习 Linux 系统中进程创建与控制有关的编程和调试技术
= 实验过程
== 学习使用库
学习使用`readline`等库
#{
    let dirname = "test/"
    let lang = "c"
    zebraw-read(dirname + "readline.c", "c")
    zebraw-read(dirname + "sys_fork_wait.c", "c")
    zebraw-read(dirname + "sys_pipe_dup2.c", "c")
    zebraw-read(dirname + "sys_open_rw.c", "c")
    zebraw-read(dirname + "sys_signal_kill.c", "c")
}
== 学习示例程序
学习示例程序,理解其实现细节
#{
    let dirname = "example/"
    let srcDir = "src/"
    zebraw-read(dirname + srcDir + "simshell.c", "c")
}
== 实现独立实验
实现独立实验,完成实验要求
#{
    let srcDir = "src/"
    zebraw-read(srcDir + "main.c", "c")
}
== 编译
使用 xmake 构建
#zebraw-read("xmake.lua", "lua")
== 运行
分别运行示例程序和独立实验,验证其功能
```bash
xmake run main
# echo here
here
# echo ls > tmp.txt
# cat tmp.txt
ls
# cat tmp.txt | wc
      1       1       3
# echo echo ls | ./main
ls
#
```
= 实验总结
== 改进点
- 支持更多的内置命令,如`cd`、`export`、`alias`等
- 支持命令替换和变量替换
- 支持更复杂的命令行编辑功能,如光标移动、文本删除等
- 支持更丰富的提示信息,如当前目录、Git 分支等
- 支持作业控制,如暂停、恢复、后台运行等
== 实现思路
- 内置命令可以在解析命令行时识别,并在父进程中直接执行,而不是通过`execvp`调用外部程序
- 命令替换和变量替换可以在解析命令行时进行,使用类似于`$(...)`和`$VAR`的语法来识别,并调用相应的函数来获取替换结果
- 命令行编辑功能可以通过使用`readline`库来实现,该库提供了丰富的编辑功能和历史记录支持
- 提示信息可以在显示提示符时动态生成,获取当前目录、Git 分支等信息,并将其包含在提示符字符串中
- 作业控制可以通过使用`waitpid`和信号处理来实现,允许用户暂停、恢复和管理后台作业
