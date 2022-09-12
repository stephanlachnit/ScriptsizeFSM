# Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
# SPDX-License-Identifier: MIT

project('scriptsizefsm', 'cpp',
  version: '1.0.0',
  default_options: ['werror=true', 'warning_level=3'])

scriptsizefsm_inc = include_directories('.')

scriptsizefsm_dep = declare_dependency(
  include_directories: scriptsizefsm_inc,
)

install_headers('scriptsizefsm/scriptsizefsm.hpp', preserve_path: true)

subdir('tests')

# examples
build_examples = get_option('build_examples')

ex_simple_switch_exe = executable('simple_switch', 'examples/simple_switch.cpp',
  dependencies: scriptsizefsm_dep,
  build_by_default: build_examples)

ex_extended_switch_exe = executable('extended_switch', 'examples/extended_switch.cpp',
  dependencies: scriptsizefsm_dep,
  build_by_default: build_examples)

ex_multiple_instances_exe = executable('multiple_instances', 'examples/multiple_instances.cpp',
  dependencies: scriptsizefsm_dep,
  build_by_default: build_examples)
