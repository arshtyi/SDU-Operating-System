#import "@preview/ezexam:0.3.1": *
#import "@preview/zebraw:0.6.1": *
#import "@preview/subpar:0.2.2"
#import "@preview/fletcher:0.5.8" as fletcher: diagram, edge, node
#import "@preview/pinit:0.2.2": *
#import "@preview/fancy-units:0.1.1": add-macros, fancy-units-configure, num, qty, unit
// #import "@preview/itemize:0.2.0" as el

#set page(height: auto)
#set par(justify: true)
#set smartquote(quotes: "\"\"")
#show: zebraw.with(lang: false)
#show: setup.with(
    mode: EXAM,
    resume: false,
    // ref-color: black,
    // list-spacing: 1.5em,
    enum-spacing: 1.5em,
    // line-height: 1em,
)
#show link: it => text(fill: blue.darken(20%), underline(it))
// #show strong: it => text(weight: "bold", it)
#let question = question.with(supplement: "Q ", ref-on: true, show-ref-prefix: false)
// #show: el.default-enum-list
// #show: el.config.ref

#let (B, TB) = (
    unit[B],
    unit[TB],
)

#title[
    山东大学计算机科学与技术学院 \
    24 数据、智能操作系统课后作业
]
#notice(
    [出于方便使用#link("https://github.com/gbchu/ezexam", "gbchu/ezexam:0.3.1")作模板.],
    [源码:#link("https://github.com/arshtyi/SDU-Operating-System").],
)

= No.1
#question[
    提交以下命令在`$HOME`下的输出信息截图(过长则截取前10行):
    ```bash
    ls -l
    ps -aux
    pwd
    ```
]

#question[
    说明@1-1-1 中命令的功能.
]

#question[
    回答下面关于GPL LICENSE的问题:
    + GPL LICENSE与传统商业软件LICENSE的区别.
    + 学习使用这些软件相比于使用传统商业软件的优势.
    + GPL LICENSE意味着免费吗?为什么?
]

#question[
    AI 时代,我们会需要什么样的操作系统?
]
= No.2
#question[
    $1$进制使用$1$的数量来表示数字的大小.下面给出了实现"$3+4$"的图灵机纸带及其操作过程, 请写出实现"$2+3$"的图灵机纸带及其操作过程.
    #align(center)[
        #show figure: set block(breakable: true)
        #show table.cell: it => {
            if (it.y == 10 - 1 and it.x > 0) {
                text(fill: red.darken(20%), it)
            } else { it }
        }
        #figure(
            table(
                columns: 21,
                table.cell(rowspan: 4)[Stage 1],
                [...],
                [$X$],
                [$1$],
                [$1$],
                [$1$],
                [$+$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$=$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [...],

                [...],
                [$X$],
                [$X$],
                [$1$],
                [$1$],
                [$+$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$=$],
                [$1$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [...],

                [...],
                [$X$],
                [$X$],
                [$X$],
                [$1$],
                [$+$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$=$],
                [$1$],
                [$1$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [...],

                [...],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$+$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$=$],
                [$1$],
                [$1$],
                [$1$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [...],

                table.cell(rowspan: 1)[Stage 2],
                [...],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$=$],
                [$1$],
                [$1$],
                [$1$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [...],

                table.cell(rowspan: 5)[Stage 3],
                [...],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$1$],
                [$1$],
                [$1$],
                [$=$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$Y$],
                [$Y$],
                [$Y$],
                [$Y$],
                [...],

                [...],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$1$],
                [$1$],
                [$=$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$Y$],
                [$Y$],
                [$Y$],
                [...],

                [...],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$1$],
                [$=$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$Y$],
                [$Y$],
                [...],

                [...],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$=$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$Y$],
                [...],

                [...],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$X$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$1$],
                [$Y$#pin(1)],
                [...],
            ),
            caption: [实现"$3+4$"的图灵机纸带及其操作过程],
        )
        #pinit-point-from(1, fill: red, offset-dx: 20pt, offset-dy: 20pt, body-dx: -20pt)[#text(
            fill: red.darken(20%),
        )[停机结果]]
    ]
    #subpar.grid(
        figure(
            table(
                columns: 2,
                inset: (x: 10pt, y: 8pt),
                table.header([状态], [描述]),
                [$A$], [$1 slash plus$判断状态],
                [$B$], [$1$的搬运状态],
                [$C$], [搬运完回退状态],
                [$H$], [停机状态],
            ),
        ),
        figure(
            diagram(
                let small-label(it) = [
                    #set text(size: 0.8em)
                    #set par(leading: 0.5em)
                    #it
                ],
                node-stroke: .1em,
                node-fill: gradient.radial(gray.lighten(80%), gray, center: (30%, 20%), radius: 80%),
                // spacing: 4em,
                node((0, 0), `B`, radius: 1.5em),
                edge((0, 0), (0, 0), small-label[$+,+,R$\ $1,1,R$\ $=,=,R$], bend: 130deg, label-pos: 0.1),
                edge("-|>", `Y,1,L`),
                node((4, 0), `C`, radius: 1.5em),
                edge(
                    (4, 0),
                    (4, 0),
                    small-label[$1,1,L$\ $=,=,L$\ $+,+,L$],
                    bend: 130deg,
                    label-pos: 0.1,
                ),
                edge((4, 0), (0, 1), `X,X,R`, "-|>"),
                node((0, 1), `A`, radius: 1.5em),
                edge((-1, .5), (0, 1), `Start`, "-|>", label-pos: -.1, label-side: center),
                edge("u", `1,X,R`, "-|>"),
                edge(`=,X,R`, "-|>", label-side: right),
                edge((0, 1), (0, 1), small-label[$+,X,R$], bend: -130deg, label-pos: 0.1),
                node((4, 1), `H`, radius: 1.5em, extrude: (-.4em, 0)),
            ),
        ),

        columns: (1fr, 2fr),
        caption: [图灵机状态描述及转换图],
    )
    #align(center)[
        #figure(
            table(
                inset: (x: 12pt, y: 8pt),
                columns: 6,
                [序号], [状态], [读入], [写入], [移动], [转向],
                [$1$], [$A$], [$1$], [$X$], [$R$], [$B$],
                [$2$], [$A$], [$+$], [$X$], [$R$], [$A$],
                [$3$], [$B$], [$+$], [$+$], [$R$], [$B$],
                [$4$], [$B$], [$1$], [$1$], [$R$], [$B$],
                [$5$], [$B$], [$=$], [$=$], [$R$], [$B$],
                [$6$], [$B$], [$Y$], [$1$], [$L$], [$C$],
                [$7$], [$C$], [$1$], [$1$], [$L$], [$C$],
                [$8$], [$C$], [$=$], [$=$], [$L$], [$C$],
                [$9$], [$C$], [$+$], [$+$], [$L$], [$C$],
                [$10$], [$C$], [$X$], [$X$], [$R$], [$A$],
                [$11$], [$A$], [$=$], [$X$], [$R$], [$H$],
            ),
            caption: [图灵机规则表],
        )
    ]
]

#question[
    请说明最精简的图灵机需要哪几条指令并给出原因.
]

#question[
    回答下面关于存储程序计算机架构的问题:
    + 这种架构的意义是什么.
    + 这种架构中必须有特权组件吗?为什么?
    + 当架构中无特权组件时,只能运行什么结构的操作系统.
]

#question[
    回答下面关于操作系统的问题(tips:第三代微内核分类):
    + 简述微内核结构、外核结构与虚拟机结构在学术定义上的区别.
    + 这些区别还有现代意义吗?为什么?
    + 试给出一个同时具备上述三种功能的操作系统.
]

#question[
    试给出一种更快速的中断响应机制.
]
= No.3
#question[
    如果代码段所在的外存可以直接执行,是否一定需要操作系统来进行其他段的装入和初始化?为什么?
]

#question[
    回答下列问题(tips: 自解压压缩文件, 内外存访问速度差距):
    + 如果程序的`.text`,`.rodata`,`.rwdata`中包含大量重复数据,是否可能通过某种方式让外存上的可执行文件更小?
    + 这样做除了节省外存空间之外还有什么好处?
    + 业界常见解决方案是什么?试给出一个工程例子.
]

#question[
    在某些操作系统中,`.text`段被加载为不可读写仅可执行.简述这样做的好处.
]

#question[
    对Linux下的可执行程序与目标文件使用`objdump -S`.简述输出内容并分析二者的区别.
]

#question[
    回答下面问题:
    + 说明栈框的内容,解释过程调用、执行和返回过程中栈框内容的变化.
    + 简述尾递归优化与栈框复用之间的关系.
]
= No.4
#question[
    应用程序在可执行文件中和在虚拟地址空间中的存放形式有什么区别?
]

#question[
    回答下列问题:
    + 运行时库调用和系统调用有什么区别和联系?为什么?
    + C语言程序可以用哪些方法结束执行并退出系统?
    + 在`main`函数`return`后,运行时库做了什么?
]

#question[
    在使用分页设计实现虚拟内存机制时,其具体实现要如何避免页表体积过大的问题?
]

#question[
    回答下面问题:
    + Linux的`*.so`和Windows的`*.dll`有什么区别?
    + `*.so`中`GOT`和`PLT`的作用是什么?
]

#question[
    阅读#link("https://www.usenix.org/system/files/osdi22-ren.pdf", "From Dynamic Loading to Extensible Transformation: An Infrastructure for Dynamic Library Transformation"),说明其提出的机制如何改进动态链接机制.
]
= No.5
#question[
    阅读课本相应内容.
]

#question[
    考虑如下线程,均在$0$时刻按照$E 1 - E 5$顺序先后提交给操作系统.
    #align(center)[
        #show table.cell.where(y: 0): it => text(white, size: 13pt, weight: "bold", it)
        #table(
            fill: (_, y) => if y == 0 { rgb("#ce858a") } else { rgb("#e7c3c5") },
            row-gutter: (range(0, 5).map(i => if (i == 0) { 0.2em } else { 0.1em })),
            column-gutter: 0.1em,
            inset: (x: 20pt, y: 10pt),
            columns: 3,
            [线程], [优先级], [运行时间],
            [$E 1$], [$3$], [$10$],
            [$E 2$], [$1$], [$1$],
            [$E 3$], [$3$], [$2$],
            [$E 4$], [$4$], [$1$],
            [$E 5$], [$2$], [$5$],
        )
    ]
    + 画出使用非抢占式FP、FCFS以及SJF时的执行图.
    + 对于(1)中的各个执行图,各个线程的等待时间和周转时间是多少?
    + 哪种调度算法的平均等待时间最短?为什么?
    + 如果采用时间固定长为$1$的FPRR算法,执行图是什么样的?
]

#question[
    相比单处理器调度,多处理器调度需要考虑哪些新的因素?为什么?怎么解决?
]

#question[
    查询指令流和线程之间以多对多关系实现的例子,使用该系统术语举例说明其中对应关系.
]

#question[
    HRRN调度算法会导致饥饿现象吗?FPRR呢?为什么?
]
= No.6
#question[
    查询资料并回答:
    + Linux中线程`nice`值的含义.
    + 用什么命令来查看和设置线程的`nice`值.
    + 为什么Linux仅允许root用户设置线程的`nice`值为负数?
]

#question[
    当存在一个外设产生大量频繁I/O请求时,说明I/O设备响应时间和CPU利用率之间为何存在矛盾.(tips:线程调度和上下文切换开销)
]

#question[
    说明平均周转时间和最大等待时间在什么情况下存在矛盾以及调度策略兼顾它们有什么困难.(tips:平均值和极端值描述的区别;系统中一般存在一系列小任务和少数长任务)
]

#question[
    查询资料并回答(tips:考虑新加入系统中的线程的`Rv`的设置):
    + Linux的CFS调度器是否会产生饥饿?
    + 为什么会有(1)中的结果?
    + 说明`min_runtime`的维护及其用途.
]

#question[
    阅读#link("https://ieeexplore.ieee.org/abstract/document/9113117", "Slite: OS Support for Near Zero-Cost, Configurable Scheduling"),回答:
    + 该文献如何实现在用户模式下调度内核级线程?
    + 这样做有什么优越效果?
]
= No.7
#question[
    阅读课本相应内容.
]

#question[
    回顾线程的RR调度算法,回答下列问题:
    + 假设线程无法主动放弃处理器,且时间片的大小固定,那么它对处理器时间的分配可能产生碎片吗?
    + 如果(1)产生,是哪种碎片?
    + 如果(1)中线程可以主动放弃处理器呢?
]

#question[
    最新的x86-64系统使用五级页表.假设快表命中时,若缓存命中则内存访问都需要$1$个时钟周期,否则需要$100$个时钟周期.(tips:快表不命中时需要访存查询表项,这也可能缓存不命中)回答下列问题:
    + 第一次访问一个不在快表中存在的页需要多少时钟周期?紧接着访问同一页又需要多少时钟周期?
    + 假设快表总是命中,而缓存命中率$c$,则平均一次内存访问需要多少时钟周期?
    + 假设缓存总是命中,而快表命中率$t$,则平均一次内存访问需要多少时钟周期?
    + 假设缓存命中率$c$和快表命中率$t$,且二者命中为独立随机事件,则平均一次内存访问需要多少时钟周期?
    + 结合上述,谈谈那些需要确定性的系统中为什么需要锁定部分快表.
]

#question[
    接@1-7-3 ,假设当页面不存在时,需要额外的$10000$个时钟周期来把它换入内存.回答下列问题:
    + 第一次访问任意一页最多需要多少时钟周期?紧接着访问同一页又需要多少时钟周期?
    + 假设快表总是命中,而页面在内存中的可能性为$m$,则平均一次内存访问需要多少时钟周期?
    + 假设页面在内存中的可能性为$m$,且其与缓存和快表的命中都是独立随机事件,则平均一次内存访问需要多少时钟周期?
]

#question[
    仿照短期调度和中期调度的工作集的定义,定义长期调度的工作集.并谈谈SSHD对比SSD、HDD的优势.
]

#question[
    阅读#link("https://ieeexplore.ieee.org/document/7108391", "Providing Task Isolation via TLB Coloring"). 回答:
    + 该文献如何实现快表着色?
    + 这样做为何能够放置不同任务的工作集互相干扰?
    + 普通缓存能进行这样的着色吗?
]

#question[
    考虑一台计算机,其在运行任务时硬盘灯狂闪、卡顿,用户检查系统性能计数器发现是请求分页大量发生.对于以下解决方案:
    + 升级或超频CPU.
    + 升级或超频内存频率.
    + 升级内存容量.
    + 更换同平台的高端主板.
    + 将硬盘组成RAID.
    + 将硬盘换成$20 #TB$HDD.
    + 将硬盘换成$1 #TB$SSD.
    + 升级显卡和显示器.
    + 改进请求分页算法.
    + 增加使用更大的页.
    + 清理不关键的后台程序,整理磁盘碎片.
    问:
    + 哪些能够产生正面效果?它们中的哪些效果最不明显?为什么?
    + 哪个是根本的解决方案?哪个是最省钱的解决方案?为什么?
]

#question[
    求证:
    + 不带抢占的SJF处理器调度算法是平均时间意义上最好的算法.
    + FIFO处理器调度算法不是任何意义上的公平调度算法.
    + FIFO页面替换算法的竞争比是$N$,其中$N$是工作集包含的总页面数.
    + LRU页面替换算法的竞争比是$N$,其中$N$含义同(3).
    + LFU页面替换算法的竞争比是$infinity$.
    + LRU和FIFO是竞争比意义上的最优在线算法.
    Tips:
    + 交换SJF序列的两个任务看是否恶化情况.
    + 反例.
    + 切分访问序列,每段含有$N$种独立页面
    + 同(3)
    + 极端反例
    + 同(3)
]
