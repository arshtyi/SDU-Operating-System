#import "@preview/ezexam:0.3.1": *

#show: setup.with(
    mode: EXAM,
    resume: false,
    heading-top: 0em,
    heading-bottom: .4em,
    line-height: .65em,
    par-spacing: .65em,
    enum-spacing: .65em,
    list-spacing: .65em,
)
#set par(justify: true)
#show link: it => text(fill: blue.darken(40%), underline(it))
#show raw: set text(font: ("JetBrains Mono", "Noto Serif CJK SC"))
#show raw.where(block: false): box.with(
    fill: luma(240),
    inset: (x: .3em, y: 0em),
    outset: (x: 0em, y: .3em),
    radius: .2em,
)
#let question = question.with(supplement: "Q", ref-on: true, show-ref-prefix: false)
#let Title = "山东大学计算机科学与技术学院操作系统期中考试"
#let author = "arshtyi"
#let date = datetime.today()
#set document(title: Title, author: author, date: date)
#title(Title)
#exam-info(info: (
    班级: "24智能",
    教师: "刘健中",
    源码: link("https://github.com/arshtyi/SDU-Operating-System", "source"),
))
#notice[24级没考。][智慧树上选项顺序每次都会变。同时评测系统也很弱智。]
#let paren = paren.with(placeholder: none)
#let fillin = fillin.with(placeholder: none)
#let choices = choices.with(r-gap: .5em)

= 单选
#question[
    下列IPC方法不能用于任两个进程间通信的是#paren[]。
    #choices[信号量][消息队列][无名管道][信号]
]

#question[
    课程中提到在系统上电时提供万年历时间的是#paren[]。
    #choices[时钟源][实时时钟][定时器][晶体振荡器]
]

#question[
    操作系统的各个组件的功能的低层次代码文本又称#paren[]。
    #choices[实现][设计][机制][策略]
]

#question[
    不能显著改善磁盘IO性能的是#paren[]。
    #choices[重排IO请求次序][在一个磁盘上设置多个分区][预读取和写合并][优化文件物理块分布]
]

#question[
    一般多线程进程中的线程不能共享#paren[]。
    #choices[堆段][代码段][栈段][数据段]
]

#question[
    关于基于时间片轮转法的线程调度的叙述错误的是#paren[]。
    #choices[当前线程的时间片用完后，该线程状态由执行态变为阻塞态][影响时间片大小的主要因素包括响应时间、系统开销和进程数量等][时间片越短，线程切换次数越多，系统开销越大][时钟中断发生后，系统会修改当前进程在时间片内的剩余时间]
]

#question[
    用户程序发出磁盘IO请求后系统的正确处理流程是#paren[]。
    #choices[用户程序系统调用处理程序中断处理程序设备驱动程序][用户程序设备驱动程序系统调用处理程序中断处理程序][用户程序系统调用处理程序设备驱动程序中断处理程序][用户程序设备驱动程序中断处理程序系统调用处理程序]
]

#question[
    无论采用何种内核结构最不可能在用户模式执行#paren[]。
    #choices[时钟中断处理程序][缺页处理程序][命令解释程序][进程调度程序]
]

#question[
    操作系统开发最需要考虑#paren[]。
    #choices[生态与兼容性][其他选项均不需要][应用场景][用户体验]
]

#question[
    在对一个数据结构的同一处的操作中，若颠倒顺序则很可能破坏数据结构的是#paren[]。
    #choices[写-读][读-写][写-写][读-读]
]

= 填空
#question[
    能操作敏感资源、仅能在内核模式下执行的指令叫#fillin[]。
]

#question[
    #fillin[]系统中，应用程序必须请求守护进程中的策略分配敏感资源，守护进程则互相通信或转而使用内核提供的机制完成这些分配操作。
]

#question[
    若给进程分配的页框数量远少于其当前工作集，则缺页率会#fillin[]，此现象称为#fillin[]。
]

#question[
    一般线程优先级数字越#fillin[]，线程优先级越高，但亦有例外。
]

#question[
    文件的索引节点内一般含#fillin[]。
]

#question[
    磁带等介质使用的文件系统的文件分配方法是#fillin[]。
]

#question[
    若系统进入不安全状态且其中指令流#fillin[]，则必定死锁。
]

#question[
    程序的两种基本链接方法中#fillin[]性能较高。
]

#question[
    线程的三个基本状态：#fillin[]、#fillin[]、#fillin[]。
]

#question[
    课程介绍的磁盘寻道算法中#fillin[]会导致饥饿。
]

= 简答
#question[
    一个网络服务器程序同时响应多个客户的多个请求，既可以采用每个请求启动一个单独进程也可以采用单进程中每个请求启动一个线程。回答：
    + 采用多进程有何优劣？
    + 采用单进程多线程有何优劣？
    + 给出一种折中方法。
]

#question[
    有一间$100$座空教室供$1,2$班自习，仅一个出入口，每次仅允许一人通过。若无空座，则欲进入的同学在门口等待有人离开产生空座再进入。且规定教室中两个班学生数量之差不超过$10$个。
    + 利用互斥锁+条件变量，类C伪代码实现```c first_enter()```、```c first_exit()```、```c second_enter()```、```c second_exit()```；
    + 封装为管程```cpp class classroom```，构造函数初始化同步变量，析构函数销毁；
    + 管程支持自定义教室最大容量```c max```、人数差值```c diff```；
    + 选用条件变量实现；
    + 规避惊群效应，使用```c wakeup()```而非```c wakeup_all()```。
]

#question[
    某$32$位x86系统按字节编址，采用二级页表分页存储管理，虚拟地址划分三段：$10$位页目录号、$10$位页表索引、$12$位页内偏移量。
    + 页和页框的大小各多少字节？进程的虚拟地址空间大小多少？可以分成多少页？
    + 假定页目录项和页表项均占$4$字节，对于一个完全映射了所有虚拟内存空间的进程，其页目录占多少页？页表占多少页？
    + 若连续三条访存指令分别访问虚拟地址`01000000H`、`01020000H`、`01000231H`，则进行地址转换时共访问多少个不同二级页表？共访问多少次二级页表？
]

= 名词解释
#question[
    死锁四个条件。
]

#question[
    操作系统。
]

#question[
    平均等待时间与响应比的定义式。
]

#question[
    文件系统的本质特征与索引节点表。
]

#question[
    同步接口与阻塞式接口定义。
]
