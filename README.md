# acidcam-cli

Command Line Version of Acid Cam for Linux with OpenCV:

Use Ctrl+C to break processing and still be able to use what has already been filtered.

 Command Line Arguments

	-l [List filters]

	-i filename [input video]

	-o filename [output video]

	-f 0,1,2 [filter list]

	-v [image visible]

	-c R,G,B [value RGB offsets]


You can use either .mov (quicktime) or .avi (xvid) for the filename extension.

Example:

	$ acidcam -i input.mov -o output.mov -f 1,2,3

Or to view progress:

	$ acidcam -i input.mov -o output.mov -f 0 -v

To List filters:

	$ acidcam -l






