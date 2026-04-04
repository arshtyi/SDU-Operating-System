set_project("experiment3")
set_version("1.0.0")

set_languages("c11")
set_toolset("gcc")
set_plat("linux")
set_arch("x86_64")
add_defines("_POSIX_C_SOURCE=200809L")
add_defines("LINUX")

target("readline")
    set_kind("binary")
    add_links("readline")
    add_files("test/readline.c")

target("simshell")
    set_kind("binary")
    add_files("example/src/simshell.c")

target("main")
    set_kind("binary")
    add_files("src/*.c")
