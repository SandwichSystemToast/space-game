# build on linux
## download
```
git clone https://github.com/SandwichSystemToast/space-game
git submodules init
git submodules update
```
## build
```
meson setup build
meson configure build <your options>
meson compile -C build
```
## install
```
meson install -C build
```
