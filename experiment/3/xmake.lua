set_project("experiment3")
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
        if test_name == "readline" then
            add_links("readline")
        end
end

target("simshell")
    set_kind("binary")
    add_files("example/src/simshell.c")

target("main")
    set_kind("binary")
    add_files("src/*.c")
