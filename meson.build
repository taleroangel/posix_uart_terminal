project(
	'posix_uart_terminal',
	'c',
	version: '1',
	default_options: ['warning_level=3'],
)

add_project_arguments(['-Wno-unused-parameter'], language: 'c')
add_project_link_arguments([], language: 'c')

inc_dir = include_directories('include')
deps = [dependency('threads', required: true)]

executable(
	'uart',
	['./src/uart/main.c', './src/uart/threads.c'],
	include_directories: inc_dir,
	dependencies: deps,
)

executable(
	'terminal',
	['./src/terminal/main.c', './src/terminal/threads.c'],
	include_directories: inc_dir,
	dependencies: deps,
)
