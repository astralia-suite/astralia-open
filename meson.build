project('astralia-open', 'cpp', version: '0.1.0',
  default_options: ['cpp_std=c++17', 'buildtype=release', 'b_lto=true', 'strip=true'])

conf_dir = get_option('prefix') / get_option('sysconfdir') / 'astralia-open'

executable('astralia-open', 'src/main.cpp',
  cpp_args: ['-fno-exceptions', '-fno-rtti', '-DDEFAULT_CONF="' + conf_dir / 'default.conf' + '"'],
  link_args: ['-static', '-Wl,--gc-sections'],
  install: true)
install_data('default.conf', install_dir: conf_dir)
