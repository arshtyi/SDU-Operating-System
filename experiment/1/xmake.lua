set_project("experiment1")
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

target("example")
    set_kind("binary")
    add_files("example/src/*.c")
    add_includedirs("example/include")

target("main")
    set_kind("binary")
    add_files("src/*.c")
    add_includedirs("include")
