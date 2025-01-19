set_defaultmode("release")
set_languages("c23") 

add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"})
add_rules("mode.release", "mode.debug")

add_includedirs("include")

target("castor")
  set_kind("static")
  add_files("src/*.c")