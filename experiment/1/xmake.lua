add_rules("mode.debug", "mode.release")

target("lab1")
    set_kind("binary")
    add_files("src/*.c")
