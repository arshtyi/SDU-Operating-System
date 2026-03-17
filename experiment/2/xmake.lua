add_rules("mode.debug", "mode.release")

target("lab2")
    set_kind("binary")
    add_files("src/*.c")
