set_project("experiment7")
set_version("1.0.0")

set_languages("c++11")
set_plat("linux")
set_arch("x86_64")
add_defines("_POSIX_C_SOURCE=200809L")
add_defines("LINUX")

target("core")
    set_kind("static")
    add_files("lib/*.cpp")
    add_includedirs("include")

target("main")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")
    add_deps("core")
