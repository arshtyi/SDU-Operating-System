add_rules("mode.debug", "mode.release")

target("lab3")
    set_kind("binary")
    add_files("src/*.c")
