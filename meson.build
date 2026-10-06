project('astralia-open', 'cpp', version: '0.1.0',
  default_options: ['cpp_std=c++17', 'buildtype=release', 'b_lto=true', 'strip=true'])

executable('astralia-open', 'src/main.cpp',
  cpp_args: ['-fno-exceptions', '-fno-rtti'],
  link_args: ['-static', '-Wl,--gc-sections'],
  install: true)
install_data('settings.conf', install_dir: get_option('sysconfdir') / 'astralia-open')
