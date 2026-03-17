#import "@preview/ezexam:0.2.9": *
#import "@preview/zebraw:0.6.1": *
#import "@preview/subpar:0.2.2"
#import "@preview/fletcher:0.5.8" as fletcher: diagram, edge, node
#import "@preview/pinit:0.2.2": *

#set page(height: auto)
#set par(justify: true)
#set smartquote(quotes: "\"\"")

#show: zebraw
#show: setup.with(
    mode: EXAM,
    resume: false,
)
#show link: it => {
    set text(fill: blue)
    underline(offset: 2pt, it)
    // underline(it)
}

#title[
    山东大学计算机科学与技术学院 \
    24 数据、智能操作系统课后作业
]

#notice(
    [出于方便使用#link("https://github.com/gbchu/ezexam", "gbchu/ezexam:0.2.9")作模板.],
    [源码:#link("https://github.com/Arshtyi/SDU-Operating-System").],
)
= No.1
#question()[
    在计算机上通过某种方式安装Linux系统并提交以下命令在`$HOME`下的输出信息截图(过长则截取前10行):
    ```bash
    ls -l
    ps -aux
    pwd
    ```
]

#question()[
    说明上述命令的功能.
]

#question()[
    回答下面关于GPL LICENSE的问题:
    + GPL LICENSE与传统商业软件LICENSE的区别
    + 学习使用这些软件相比于使用传统商业软件的优势
    + GPL LICENSE意味着免费吗?为什么?
]

#question()[
    AI 时代,我们会需要什么样的操作系统?
]
= No.2
#question()[
    $1$进制使用$1$的数量来表示数字的大小.下面给出了实现"$3+4$"的图灵机纸带及其操作过程.请写出实现"$2+3$"的图灵机纸带及其操作过程.
    #align(center)[
        // #show figure: set block(breakable: true)
        #show table.cell: it => {
            if (it.y == 10 - 1 and it.x > 0) {
                set text(fill: red.darken(20%))
                it
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
                [$Y$],
                [...#pin(1)],
            ),
            caption: [实现"$3+4$"的图灵机纸带及其操作过程],
        )
        #pinit-point-from(1, fill: red, offset-dx: 20pt, offset-dy: 20pt)[#text(fill: red.darken(20%))[停机结果]]
    ]
    #subpar.grid(
        figure(
            table(
                columns: 2,
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

#question()[
    请说明最精简的图灵机需要哪几条指令并给出原因.
]

#question()[
    回答下面关于存储程序计算机架构的问题:
    - 这种架构的意义是什么.
    - 这种架构中必须有特权组件吗?为什么?
    - 当架构中无特权组件时,只能运行什么结构的操作系统.
]

#question()[
    回答下面关于操作系统的问题:
    - 简述微内核结构、外核结构与虚拟机结构在学术定义上的区别.
        - 这些区别还有现代意义吗?为什么?
    - 试给出一个同时具备上述三种功能的操作系统(tips:第三代微内核分类).
]

#question()[
    试给出一种更快速的中断响应机制.
]
