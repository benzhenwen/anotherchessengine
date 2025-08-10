project('chmess', ['c', 'cpp'],
  version: '0.1.0'
)

logger_dep = dependency('logger', fallback: ['logger', 'logger_dep'])
glm_dep = dependency('glm', fallback: ['glm', 'glm_dep'])

executable('chmess', 'projects/demo/main.cpp',
    win_subsystem: 'windows',
    dependencies: [logger_dep, glm_dep])
    
executable('chmess_perft', 'projects/perft/main.cpp',
    win_subsystem: 'windows',
    dependencies: [logger_dep, glm_dep])


executable('genZobrist', 'generators/genZobrist/main.cpp',
    win_subsystem: 'windows',
    dependencies: [logger_dep])

executable('genBitboards', 'generators/genBitboard/main.cpp',
    win_subsystem: 'windows',
    dependencies: [logger_dep])