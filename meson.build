project('Smirk', 'c',
        version: '1.0.0',
        default_options: ['warning_level=1', 'werror=false'],
        license: 'MIT')


source_files = []
include_dirs = []


c_args = [
    '-DVERSION="@0@"'.format(meson.project_version()),
    '-Werror=format', '-Werror=format-security'
]

link_args = []


subdir('kernel')


foreach source : source_files
    source_name = '@0@'.format(source)
    message(source_name)
endforeach

kernel = executable('kernel',
                    [source_files],
                    name_suffix: 'elf',
                    include_directories: include_dirs,
                    c_args: c_args,
                    link_args: link_args)

