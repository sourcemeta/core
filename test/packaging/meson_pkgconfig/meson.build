project('core_hello', 'cpp',
        default_options : [ 'cpp_std=c++23' ])

# The whole library behind a single name, with no knowledge of how it is split
# into modules or of what any of them link against
core_dependency = dependency('sourcemeta_core', static : true, required : true)

executable('core_hello', 'hello.cc', dependencies : [ core_dependency ])
