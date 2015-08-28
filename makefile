# auto-config
ROOT_DIR = $(CURDIR)

# build rules
all: paraview kronos testapp

paraview:
	./build_paraview.sh

kronos:

testapp:
	cd ParaView-TestApp;\
		./build_testapp.sh
