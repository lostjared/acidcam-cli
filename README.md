# acidcam-cli

Command Line Version of Acid Cam for Linux with OpenCV:

Use Ctrl+C to break processing and still be able to use what has already been filtered.

To compile, first Install OpenCV on debian you can do this with:

	$ sudo apt-get install libopencv-dev

You will also require GCC or clang and autoconf, automake

	$ sudo apt-get install autoconf automake

Then download this archive and enter the directory you extracted to in the terminal then:

	$ ./autogen.sh && ./configure && make && sudo make install

After you do that if everything goes according to plan you can test it with:

	$ acidcam -l

 Command Line Arguments

	-l [List filters]

	-i filename [input video]

	-o filename [output video]

	-f 0,1,2 [filter list]

	-v [show video while being processed]

	-c R,G,B [value RGB offsets]
	
	-p plugin.so [plugin written in C++ see plugins folder]

If you install from .deb package the programs name is acidcam-cli to not conflict
with name of Acid Cam Qt program name. To install download .deb for your platform and

	$ sudo dpkg -i acidcam-cli*deb

Once you have it installed you can use either .mov (quicktime) or .avi (xvid) for the filename extension.

Example:

	$ acidcam-cli -i input.mov -o output.mov -f 1,2,3

Or to view progress:

	$ acidcam-cli -i input.mov -o output.mov -f 0 -v

To List filters:

	$ acidcam-cli -l

To use plugin:

	$ acidcam-cli -i input.mov -o output.mov -p path/to/shared.so -f plugin

or Add other filters:

	$ acidcam-cli -i input.mov -o output.mov -p path/to/shared.so -f 0,plugin,1

