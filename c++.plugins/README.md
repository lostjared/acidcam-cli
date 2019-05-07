In the subdirectories within this directory there are examples of a simple acidcam-cli plugin
its basically just an exported function with one argument.
You can use whatever OpenCV functions you want just modify the Matrix passed 
in and it will be concatinated with other filters then written to the video file.

Use a plugin like this:
	
	$ acidcam -i input_file.mov -o output_file.avi -p path/to/plugin.ac -f plugin


