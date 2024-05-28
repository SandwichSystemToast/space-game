run: compile
	./build/space-game

setup:
	meson setup build

compile:
	meson compile -C build
