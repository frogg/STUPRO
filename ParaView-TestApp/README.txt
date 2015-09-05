################
# HOWTO BUILD:
################

	1. > mkdir bin
		it is necessary to build into a subdir called bin as this dir is ignored by git (see gitignore)

	2. > cd bin/

	3. > ccmake ..
		- run through the standard cmake procedure
		- make sure to set ParaView_DIR to your paraview build directory
		- if there are any problems you can try the following:
			* make a clean build. This means removing any CMake cache including the bin-directory
				created in step 1.
			* make sure to run 'c' (configure) at least two times and 'g' (generate) afterwards.
	4. > make
		This should build the app and you are done.
