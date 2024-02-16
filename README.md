# acidcam-cli

Acidcam-CLI transforms videos into works of art, leveraging a vast array of filters to create unique visuals known as Acid Glitches. This tool can be utilized either as a standalone application or in conjunction with other software to foster the creation of artistic images and videos. Designed for enthusiasts and Glitch artists alike, Acidcam-CLI serves as a creative catalyst.

System Requirements
For optimal performance and access to the full suite of filters, ensure your system has at least 8 GB of RAM.

Availability
Acidcam-CLI is available across multiple platforms including macOS, Linux, and Windows, catering to a wide audience of artists and developers.

Project Goals
This project aims to entertain and inspire, providing a foundation for Glitch artists and creative individuals to explore and expand their artistic expressions.

Filters
With over 1,000 filters to explore, Acidcam-CLI offers endless possibilities for artistic experimentation. These filters, or code snippets, manipulate images in various ways, allowing users to achieve distinctive and captivating results.

Installation and Use
For Linux/Unix with OpenCV:

To compile, first Install OpenCV if you are using debian you can do this with:

	$ sudo apt-get install libopencv-dev g++ cmake autoconf automake libtool git pkg-config 

Download libacidcam from: https://github.com/lostjared/libacidcam

Compile and install the library

    $ mkdir -p build && cd build
    $ cmake .. && make -j4 && sudo make install

If you are running on Linux you should use this command so it can find the library:

	$ sudo ldconfig

Then download this archive and enter the directory you extracted to in the terminal then:

    $ mkdir -p build && cd build
    $ cmake .. && make -j4 && sudo make install

** PYTHON SUPPORT STILL IN DEVELOPMENT **

If you wish to use Python scripts for filters:
First download and install Metacall http://github.com/metacall install
then configure and compile. You will also need to install the Metacall python module with pip3

    $ cmake .. -D ENABLE_METACALL=ON
    $ make -j4 && sudo make install

If are going to use acidcam with Metacall you need to set the shell envoriment variable LOADER_SCRIPT_PATH to the path with the python code you want to use an example:

	$ export LOADER_SCRIPT_PATH=/home/jared/scripts

Also set the library path for the metacall shared object files (.so)

	$ export PORT_LIBRARY_PATH=/usr/local/lib

After you do that if everything goes according to plan you can test it with:

	$ acidcam-cli -l

NOTE: Some of the filters that contain either a 720 or 1080 at the end require a lot of ram if you do not have enough the program will exit.

If you are using a high resolution the program outputs very high bitrate video for MPEG-4. To be able to play it back you can load it into Final Cut and export as H.264, use Handbrake to lower the bitrate, or use ffmpeg. The command I use to compress 4K Acid Cam videos is:

    $ ffmpeg -i "input.file.mp4" -c:v libx265 -tag:v hvc1 -crf 18 output.file.mp4

This command will compress and retain a good deal of the files quality.

Remember to put quotes around arguments that use spaces for like file names etc..

 Command Line Arguments

	-l [List filters]

	-L [List Filters by Name]

	-i filename [input video]

	-o filename [output video]
	
	-a "filename1.mov, filename2.mov ..." [additional files to apply effect]

	-f 0,1,2 [filter list]

	-e flip input frame

	-v [show video while being processed]

	-c R,G,B [value RGB offsets]
	
	-p plugin.py [plugin written in Python see plugins folder]

	-x [list plugins in current directory (end with .py)]
	
	-n index [uses plugin index from list printed out with -x]

	-g filename [uses image file for blend with image filters]
	
	-b brightness [value from 0-255]

	-m gamma [value from 0-255]

	-s saturation [value from 0-255]

	-r color_map [value from 1-12]

	-k colorkey_image [filename for color keyed image (uses 255,0,255)]

	-S subfilter [subfilter used in some filters]

	-u Resolution to Resize ex 1920x1080 or 1280x720

	-F frame_rate force output frame rate

	-I frames skip any number of frames outputted to file

	-P path for metacall install location (usually /usr/local)

	-T thread count for Multithreaded filters (default is 4)

	-J x264 to  output as x264 (pipe to FFmpeg)

	-J x265 to  output as x265 (pipe to FFmpeg)

	-Q crf 

	-j second layer video input



List of the Different Color maps you can apply with -r:

	1 Autum
	2 Bone
	3 Jet
	4 Winter
	5 Rainbow
	6 Ocean
	7 Summer
	8 Spring
	9 Cool
	10 HSV
	11 Pink
	12 Hot

	$ acidcam-cli -i test.mov -o output.mov -f 1,2 -k /path/to/colorkey.png

You can also apply a color key image 
all the pixels in the image that have the color value 255,0,255 will contain the filtered data rest of the pixels will be the same

Once you have it installed you can use either .mov (quicktime) or .avi (xvid) for the filename extension.

Example:

	$ acidcam-cli -i input.mov -o output.mov -f 1,2,3

or resize video

	$ acidcam-cli -i input.mov -o output.mov -f 1,2,3 -u 1920x1080

use a subfiler give the index number followed by a colon and the subfilter it will use like this:
	
	$ acidcam-cli -i input.mov -o output.mov -f 962:3

to use a subfilter and other files use:

	$ acidcam-cli -i input.mov -o output.mov -f 962:0,10,5

or to view progress:

	$ acidcam-cli -i input.mov -o output.mov -f 0 -v

to list filters:

	$ acidcam-cli -l

to list plugins:

	$ acidcam-cli -x

to use plugin:

	$ acidcam-cli -i input.mov -o output.mov -p path/to/shared.acf -f plugin

or use:

	$ acidcam-cli -x

lists the plugins found then use -n index

	$ acidcam-cli -i input.mov -o output.mov -n 0 -f plugin

or add other filters:

	$ acidcam-cli -i input.mov -o output.mov -p path/to/shared.acf -f 0,plugin,1

or apply a color map:

	$ acidcam-cli -i input.mov -o output.mov -f 28,1 -r 3
	
or apply brightness/gamma/saturation

	$ acidcam-cli -i input.mov -o output.mov -b 100 -m 2 -s 25 -f 0,1


a example of using acidcam:

	$ acidcam-cli -i jaredoffice.mov -o blend.mov -g pencil.nin.png -f 31,168 -a "peace.1080p.mov, test.mov" -e -r 3 -c 0,0,50

another Example:

	$ acidcam-cli -i videofile.mov -o videooutput.mov -P /usr/local -n 0 -f plugin

