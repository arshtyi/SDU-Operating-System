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
#let lab-title = "实验6-死锁问题"
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
- 观察死锁现象并学习解决死锁的方法
- 练习构造管程和条件变量来解决问题
= 实验过程
== 学习示例程序
抄写并学习给出的示例程序:
#{
    let dirname = "example/"
    let includeDir = "include/"
    let libDir = "lib/"
    let srcDir = "src/"
    let lang = "cpp"
    zebraw-read(dirname + includeDir + "dp.h", lang)
    zebraw-read(dirname + srcDir + "dp.cpp", lang)
}
== 实现独立实验
设计并实现独立实验:
#{
    let includeDir = "include/"
    let libDir = "lib/"
    let srcDir = "src/"
    let lang = "cpp"
    zebraw-read(includeDir + "rail.h", lang)
    zebraw-read(libDir + "rail.cpp", lang)
    zebraw-read(srcDir + "main.cpp", lang)
}
== Build & Run
使用xmake构建:
#zebraw-read("xmake.lua", "lua")
首先进行示例实验的测试:
#figure(image("fig/1.png"))
然后进行独立实验的测试:
#figure(image("fig/2.png"))
= 实验总结
- 观察
    + 示例实验中,终端主要出现 `hungry / eating / thinking` 三类状态输出,能够观察到进程在"饥饿-唤醒-就餐-思考"之间循环.
    + 从输出节奏看,示例程序在任一时刻只允许非常保守的就餐推进,系统不会卡死在所有进程都等待的状态,因此演示了"可运行的同步控制",但并未体现经典哲学家问题中"非相邻者可并行就餐"的核心特征.
    + 独立实验输出了 `[ENTER]/[LEAVE]`、`on_track` 和 `waiting`,说明同一时刻轨道上方向一致且当双向都有等待时,方向会在批次阈值附近切换,整体吞吐与公平性可观察、可调参.
- 示例实验是否真正模拟了哲学家就餐问题
    :不完全是.经典哲学家问题允许"互不相邻的哲学家并行就餐";而示例程序通过全局互斥进入管程,并采用非常保守的状态推进,实测表现更接近"串行化的安全版本",重点在演示管程/条件同步,而不是最大化并行就餐.
- 为什么示例程序不会产生死锁
    + `pickup`/`putdown` 在共享状态检查和修改时受同一把锁保护,避免了并发竞争导致的环路等待构造.
    + 进程不能满足条件时会释放锁并在条件队列阻塞,不会长期占有锁等待其他资源,从而破坏"请求并保持".
    + 状态转移由管程统一控制,邻居条件满足后由 `Signal` 唤醒,系统始终存在推进路径.
- 为什么会出现死锁和饥饿
    + 死锁本质来自"互斥、请求并保持、不可剥夺、循环等待"同时成立;资源获取顺序不一致时最易触发.
    + 饥饿本质是"长期得不到调度或资源分配",即系统有进展但个体无进展;常见于无公平策略的唤醒/调度.
- 怎样在实验中造成并表现死锁与饥饿
    + 死锁:将示例改为"先拿左筷子再拿右筷子"的分离加锁且不引入统一顺序/仲裁者,易形成环形等待;运行后可见所有进程停在等待态、无新的进餐输出.
    + 饥饿:在唤醒策略中长期偏向同一侧(或同一哲学家),另一侧持续 `hungry`/`waiting` 但很少进入 `eating`;日志体现为一侧反复通过,另一侧等待时间不断累积.
- 管程避免死锁与饥饿的机理
    + 管程用互斥入口保证共享状态修改原子化,先判断条件再阻塞,减少竞争窗口.
    + 条件变量把"等待队列"显式化,使阻塞与唤醒围绕谓词进行,不靠忙等.
    + 若再叠加公平策略(如你的 `max_batch + prefer_dir_`),可把"可推进"扩展为"近似公平推进",显著抑制饥饿.
- 对管程概念的理解
    :管程不是"锁的语法糖",而是"共享状态 + 互斥规则 + 条件队列 + 状态不变式"的整体封装.核心价值是把并发正确性从"调用者自觉"变成"对象内部约束".
- 条件变量与信号量的不同
    + 条件变量不保存资源计数,必须与互斥锁和条件谓词配合使用;语义是"条件不满足则睡眠,满足后被唤醒再竞争锁".
    + 信号量是带计数的同步原语,可脱离特定互斥区单独使用;语义更底层,既能做互斥也能做资源配额.
    + 条件变量强调"等待某个状态",信号量强调"管理某个计数资源".
- 为什么管程中优先用条件变量而非直接用信号量
    + 条件变量与"管程谓词"一一对应,可读性和可验证性更高.
    + `wait` 会原子地释放锁并阻塞,唤醒后再重新持锁检查谓词,天然匹配管程模型.
    + 直接用信号量容易把"资源计数"和"状态条件"混杂,增加丢唤醒、错唤醒和维护难度.
- 示例实验中条件变量与锁的作用
    + 示例为"每个哲学家一个条件变量(`self[i]`)"的样式,本质是按个体划分等待队列.
    + 其等待/唤醒流程表现为 Mesa 风格(发信号者继续执行,被唤醒者后续再竞争锁进入管程).
    + 锁的作用是保护 `state[]` 的一致性,并保证"检查条件-阻塞/唤醒"与状态更新之间的原子性.
- 独立实验如何解决单行道问题、如何构造管程对象
    + 你将单行道抽象为 `RailMon` 管程:`enter/leave` 是唯一入口,所有共享变量在 `mtx_` 保护下读写.
    + 用 `active_dir_` 保证同一时刻仅单方向通行;`on_track_` 记录占用;`waiting_[2]` 记录两侧等待量.
    + 用 `phase_count_` 与 `max_batch_` 限制单方向连续放行批次,避免一侧无限"霸道".
    + 用 `prefer_dir_` 在双侧都等待时做方向轮换偏好,结合 `cv_[2]` 精准唤醒,提升公平性并降低饥饿概率.
    + 因此该实现同时满足安全性(无对向冲突)与活性(系统持续推进),并通过批次+偏好策略兼顾吞吐与公平.
