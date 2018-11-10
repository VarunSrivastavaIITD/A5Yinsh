project('A5Yinsh', 'cpp', version : '1.0.0', license : 'MIT', default_options : ['cpp_std=c++11'])

staticdep = dependency('boost', modules : ['thread', 'system'], static : true)

src = ['main.cpp', 'state.cpp', 'move.cpp', 'heuristic.cpp']

test_src = ['test_heuristic.cpp', 'state.cpp', 'move.cpp', 'heuristic.cpp']

executable('yinsh', sources: src, dependencies : staticdep)
executable('testheuristic', sources: test_src, dependencies: staticdep)