set_project("experiment5")
set_version("1.0.0")

set_languages("c11")
set_toolset("gcc")
set_plat("linux")
set_arch("x86_64")
add_defines("_POSIX_C_SOURCE=200809L")
add_defines("LINUX")

for _, test_file in ipairs(os.files("test/*.c")) do
    local test_name = path.basename(test_file)
    target(test_name)
        set_kind("binary")
        add_files(test_file)
end

target("example_ipc")
    set_kind("static")
    add_files("example/lib/*.c")
    add_includedirs("example/include")

for _, example_file in ipairs(os.files("example/src/*.c")) do
    local example_name = path.basename(example_file)
    target(example_name)
        set_kind("binary")
        add_files(example_file)
        add_includedirs("example/include")
        add_deps("example_ipc")
end

target("core_lib")
    set_kind("static")
    add_files("lib/*.c")
    add_includedirs("include")

target("main")
    set_kind("binary")
    add_files("src/main.c")
    add_includedirs("include")
    add_deps("core_lib")
