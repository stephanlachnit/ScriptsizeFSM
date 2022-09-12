# Copyright © 2022 Stephan Lachnit <stephanlachnit@debian.org>
# SPDX-License-Identifier: MIT

test_simple_switch_exe = executable('simple_switch', 'simple_switch.cpp',
  dependencies: scriptsizefsm_dep,
  build_by_default: false)
test('simple_switch', test_simple_switch_exe)

test_extended_switch_exe = executable('extended_switch', 'extended_switch.cpp',
  dependencies: scriptsizefsm_dep,
  build_by_default: false)
test('extended_switch', test_extended_switch_exe)

test_multiple_instances_exe = executable('multiple_instances', 'multiple_instances.cpp',
  dependencies: scriptsizefsm_dep,
  build_by_default: false)
test('multiple_instances', test_multiple_instances_exe)
