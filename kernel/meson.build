include_dirs += include_directories('include')

link_args += ['-T', meson.current_source_dir() / 'linker.ld']
source_files += files('fbcon.c')
source_files += files('kmain.c')

if host_machine.cpu_family() == 'x86'
    c_args += ['-D_STIVALE2_SPLIT_64']
endif
