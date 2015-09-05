################
# HOWTO BUILD:
################

	1. > mkdir bin
		it is necessary to build into a subdir called bin as this dir is ignored by git (see gitignore)

	2. > nano pvdir
		this file is used to determine your paraview build dir. Write a single line into it containing
		the path to the paraview build directory. This may look like the following line:
			/usr/dev/paraview-v4.3.1-build
		If this works you may skip to the next step.
		If you run into problems here have a look at the following line:
			file (STRINGS "pvdir" ParaView_DIR)
		This is used to read the directory. You may consult the cmake doc to do your setup.

	3. > cd bin/

	4. > ccmake ..
		- run through the standard cmake procedure
		- if there are any problems you can try the following:
			* make a clean build. This means removing any CMake cache including the bin-directory
				created in step 1.
			* make sure to run 'c' (configure) at least two times and 'g' (generate) afterwards.

	5. > make
		This should build the app and you are done.
