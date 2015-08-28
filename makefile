# auto-config
ROOT_DIR = $(CURDIR)

# build rules
all: paraview kronos

paraview:
	./build_paraview.sh

kronos:
