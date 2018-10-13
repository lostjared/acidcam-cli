/*
 * Acid Cam v2 - OpenCV Edition
 * written by Jared Bruni ( http://lostsidedead.com / https://github.com/lostjared )
 
 
 GitHub: http://github.com/lostjared
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
 BSD 2-Clause License
 
 Copyright (c) 2017, Jared Bruni
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */
#include"acidcam-cli.hpp"
#include<cstdlib>
#include<cstring>
#include<cctype>
#include<dirent.h>
#include<vector>
#include<sys/stat.h>

/* required to be declared in source file */
/*
 Command Line Arguments
 -l List filters
 -L list filters sorted by name
 -i input video
 -o output video
 -f filter list
 -S subfilter
 -v image visible
 -c r,g,b set colors
 -p plugin
 -g image file for blend with image filters
 -b brightness
 -m gamma
 -s saturation
 -r colormap
 -k color key image
 -a additional videos
 -e source flip video frame
 -S subfilter
 -X concat add type set to Xor
 -A concat add type set to Add
 */
cmd::AC_Program program;
std::string secondVideoFile;

void custom_filter(cv::Mat &frame) {}
void listPlugins(std::string path, std::vector<std::string> &files);

void plugin_callback(cv::Mat &frame) {
    if(program.isPluginLoaded()) {
    	program.callPlugin(frame);
    } else {
        std::cerr << "acidcam: Error no plugin loaded exiting...\n";
        exit(EXIT_FAILURE);
    }
}

template<typename F>
void getList(std::string args, std::vector<int> &v, F func);
void getStringList(std::string args, std::vector<std::string> &v);
bool parseRes(const std::string &text, int &fw, int &fh);

void listFilters(bool sorted = false) {
    std::cout << "List of Filters by Index\n";
    std::vector<std::string> filter_names;
    for(unsigned int i = 0; i < ac::draw_max-4; ++i)
        filter_names.push_back(ac::draw_strings[i]);
    
    if(sorted == true) sort(filter_names.begin(), filter_names.end());
    
    for(unsigned int i = 0; i < filter_names.size(); ++i) {
        int index = ac::filter_map[filter_names[i]];
        std::cout << std::setw(4) << std::left << index << std::setw(50) << std::left << filter_names[i] << "\n";
    }
    std::cout << "\nList of Color Maps\n";
    for(unsigned int i = 1; i <= 12; ++i) {
        std::cout << std::setw(4) << std::left << i << std::setw(50) << std::left << cmd::colorMaps[i-1] << "\n";
    }
}

void toLower(std::string &text) {
    for(unsigned int i = 0; i < text.length(); ++i) {
        text[i] = tolower(text[i]);
    }
}

template<typename F>
void getList(std::string args, std::vector<unsigned int> &v, F func) {
    std::string number;
    unsigned int pos = 0;
    while(pos < args.length()) {
        if(args[pos] != ',')
            number += args[pos];
        if(args[pos] == ',') {
            unsigned int value = 0;
            if(number == "plugin")
                value = ac::filter_map["Plugin"];
            else
                value = atoi(number.c_str());
            number = "";
            if(func(value))
            	v.push_back(value);
        }
        ++pos;
    }
    if(number.length() > 0) {
        unsigned int value = 0;
        if(number == "plugin")
            value = ac::filter_map["Plugin"];
        else
            value = atoi(number.c_str());
        
        if(func(value))
        	v.push_back(value);
    }
}

void getStringList(std::string args, std::vector<std::string> &v) {
    std::string text;
    unsigned int pos = 0;
    while(pos < args.length()) {
        if(args[pos] != ',')
            text += args[pos];
        if(args[pos] == ',') {
            v.push_back(text);
            text = "";
        }
        ++pos;
    }
    if(text.length() > 0) {
        v.push_back(text);
    }
}

void listPlugins(std::string path, std::vector<std::string> &files) {
    DIR *dir = opendir(path.c_str());
    if(dir == NULL) {
        std::cerr << "Error could not open directory: " << path << "\n";
        return;
    }
    dirent *file_info;
    while( (file_info = readdir(dir)) != 0 ) {
        std::string f_info = file_info->d_name;
        if(f_info == "." || f_info == "..")  continue;
        std::string fullpath=path+"/"+f_info;
        struct stat s;
        
        lstat(fullpath.c_str(), &s);
        if(S_ISDIR(s.st_mode)) {
            if(f_info.length()>0 && f_info[0] != '.')
                listPlugins(path+"/"+f_info, files);
            
            continue;
        }
        if(f_info.length()>0 && f_info[0] != '.') {
            if(fullpath.rfind(".acf") != std::string::npos) {
            	files.push_back(fullpath);
                continue;
            }
        }
    }
    closedir(dir);
}

void control_Handler(int sig) {
	program.stop();
    std::cout << "\nacidcam: Signal caught stopping...\n";
}

bool parseRes(const std::string &text, int &fw, int &fh) {
    auto pos = text.find("x");
    if(pos == std::string::npos)
        return false;
    std::string left = text.substr(0, pos);
    std::string right = text.substr(pos+1, text.length()-pos);
    fw = atoi(left.c_str());
    fh = atoi(right.c_str());
    std::cout << "parsed Resolution: " << fw << "x" << fh << "\n";
    if(fw > 320 && fh > 240)
        return true;
    return false;
}

void output_software_info(std::string name) {
    std::cout << name << " " << APP_VERSION << " filters version: " << ac::version << "\n\nCommand Line Arguments\n-l List filters\n-L list filters sorted by name\n-i input video\n-o output video\n-f filter list\n-v image visible\n-c r,g,b set colors\n-p plugin\n-g image file for blend with image filters\n-b brightness\n-m gamma\n-s saturation\n-r colormap\n-k color key image\n-a additional videos\n-A add together frames for multiple video files.\n-C add together by scale of how many videos\n-O Use or to concat multiple videos\n-N use and to concat multiple video files.\n-X Xor frames for multiple video files.\n-e source flip video frame\n-S subfilter\n-u Resolution ex: 1920x1080\n-F force fps\n-I skip frames\n";
}

/* main function */
int main(int argc, char **argv) {
    ac::fill_filter_map();
    std::vector<std::string> files_list;
    ac::setPlugin(plugin_callback);
    std::string input,output;
    std::vector<unsigned int> filter_list;
    std::vector<unsigned int> col;
    bool visible = false;
    cmd::File_Type ftype;
    int bright_ = 0, gamma_ = 0, sat_ = 0, color_m = 0;
    if(argc > 1) {
        int opt = 0;
        while((opt = getopt(argc, argv, "Lli:o:f:vc:p:xn:hg:b:m:s:r:k:a:eS:u:CXANOF:I:")) != -1) {
            switch(opt) {
                case 'h':
                    output_software_info(argv[0]);
                    exit(EXIT_SUCCESS);
                    break;
                case 'l':
                    listFilters();
                    exit(EXIT_SUCCESS);
                    break;
                case 'L':
                    listFilters(true);
                    exit(EXIT_SUCCESS);
                    break;
                case 'i':
                    input = optarg;
                    break;
                case 'r':
                    color_m = atoi(optarg);
                    if(!(color_m > 0 && color_m <= 12)) {
                        std::cerr << "acidcam: Error color Map is not in range: 1-12\n";
                        exit(EXIT_FAILURE);
                    }
                        
                    break;
                case 'b':
                    bright_ = atoi(optarg);
                    if(bright_< 0 || bright_ > 255) {
                        std::cerr << "acidcam: Error range for brigthness is 0-255.\n";
                        exit(EXIT_FAILURE);
                    }
                    break;
                case 'm':
                    gamma_ = atoi(optarg);
                    if(gamma_ < 0 || gamma_ > 255) {
                        std::cerr << "acidcam: Error gamma is out of range should be 0-255\n";
                        exit(EXIT_FAILURE);
                    }
                    break;
                case 's':
                    sat_ = atoi(optarg);
                    if(sat_ < 0 || sat_  > 255) {
                        std::cerr << "acidcam: Error Saturation is out of range should be 0-255\n";
                        exit(EXIT_FAILURE);
                    }
                    break;
                case 'a': {
                    secondVideoFile = optarg;
                    getStringList(secondVideoFile,files_list);
                }
                    break;
                case 'o': {
                    std::string output_l = optarg;
                    output = optarg;
                    toLower(output_l);
                    auto pos = output_l.find(".mov");
                    if(pos == std::string::npos) {
                        auto pos2 = output_l.find(".avi");
                        if(pos2 == std::string::npos) {
                        	output += ".mov";
                        	std::cerr << "acidcam: File type not specified using default: " << output << "\n";
                        } else {
                            ftype = cmd::File_Type::AVI;
                        }
                    } else {
                        ftype = cmd::File_Type::MOV;
                    }
                }
                    break;
                case 'f': {
                    std::string args = optarg;
                    auto pos = args.find(",");
                    if(pos == std::string::npos && args.length() > 0) {
                        unsigned int value = 0;
                        std::string plug = optarg;
                        if(plug == "plugin")
                            value = ac::filter_map["Plugin"];
                        else
                            value = atoi(optarg);
                        if(value < ac::draw_max-4) {
                            filter_list.push_back(value);
                        } else {
                            std::cerr << "acidcam: Error filter out of bounds..\n";
                        }
                    } else if(args.length() == 0) {
                        std::cerr << "acidcam: Error requires at least one filter.\n";
                        exit(EXIT_FAILURE);
                        
                    } else {
                        // list of filters
                        getList(args, filter_list, [](unsigned int value) {
                            if(value < ac::draw_max-4)
                            	return true;
                            std::cerr << "acidcam: Error value must be one of the listed integer filter indexes.\n";
                            exit(EXIT_FAILURE);
                        });
                    }
                }
                    break;
                case 'c': {
                    std::string colors = optarg;
                    auto pos = colors.find(",");
                    if(pos == std::string::npos) {
                        std::cerr << "acidcam: Requires three RGB values separeted by commas.\n";
                        exit(EXIT_FAILURE);
                    }
                    getList(colors, col, [](unsigned int value) {
                        if(value <= 255)
                            return true;
                        std::cerr << "acidcam: Error color value: " << value << " should be between 0-255\n";
                        exit(EXIT_FAILURE);
                    });
                    if(col.size() != 3) {
                        std::cerr << "acidcam: Requires three RGB values separeted by commas.\n";
                        exit(EXIT_FAILURE);
                    }
                    break;
                }
                case 'v':
                    visible = true;
                    break;
                case 'p': {
                    if(program.loadPlugin(optarg)) {
                        std::cout << "acidcam: Loaded plugin: " << optarg << "\n";
                    } else {
                        std::cerr << "acidcam: Could not load plugin... exiting...\n";
                        exit(EXIT_FAILURE);
                    }
                }
                    break;
                case 'x': {
                    std::vector<std::string> v;
                    listPlugins(".", v);
                    if(v.size() > 0) {
                        std::cout << "acidcam: Plugins found\n";
                        for(unsigned int i = 0; i < v.size(); ++i) {
                            std::cout << std::setw(4) << std::left << i << std::setw(50) << std::left << v[i] << "\n";
                        }
                        
                    } else {
                        std::cout << "acidcam: No plugins fond\n";
                    }
                    exit(EXIT_SUCCESS);
                }
                    break;
                case 'n': {
                    std::vector<std::string> v;
                    listPlugins(".", v);
                    int plug = atoi(optarg);
                    if(v.size() > 0 && (plug >= 0 && plug < v.size())) {
                        if(program.loadPlugin(v[plug])) {
                            std::cout << "acidcam: Loaded plugin: " << v[plug] << "\n";
                        } else {
                            std::cerr << "acidcam: Error could not load plugin: " << v[plug] << "\n";
                        }
                    }
                }
                    break;
                case 'g':
                    blend_image = cv::imread(optarg);
                    if(blend_image.data == 0) {
                        std::cerr << "acidcam: Error could not find file: " << optarg << "\n";
                        exit(EXIT_FAILURE);
                    }
                    blend_set = true;
                    program.setImageFilename(optarg);
                    break;
                case 'k':
                    color_image = cv::imread(optarg);
                    if(color_image.empty()) {
                        std::cerr << "acidcam: Could not load image..\n";
                        exit(EXIT_FAILURE);
                    }
                    std::cout << "acidcam: Color Key Set to R: 255 G: 0 B: 255 w/ image: " << optarg << "\n";
                    colorkey_set = true;
                    break;
                case 'e':
                    program.setFlip(true);
                    break;
                case 'S': {
                    int subf = atoi(optarg);
                    if(subf >= 0 && subf <= ac::draw_max-6) {
                        ac::setSubFilter(subf);
                        std::cout << "acidcam: Subfilter set to: " << ac::draw_strings[subf] << "\n";
                    }
                }
                    break;
                    
                case 'u': {
                    std::string text = optarg;
                    int frame_width = 0, frame_height = 0;
                    if(!parseRes(text, frame_width, frame_height)) {
                        std::cerr << "acidcam: Error Resolution String invalid..\n";
                        exit(EXIT_FAILURE);
                    }
                    program.setResolutionResize(frame_width, frame_height);
                }
                    break;
                case 'C':
                    program.addType(cmd::AddType::AT_ADD_SCALE);
                    break;
                case 'X':
                    program.addType(cmd::AddType::AT_XOR);
                    break;
                case 'A':
                    program.addType(cmd::AddType::AT_ADD);
                    break;
                case 'N':
                    program.addType(cmd::AddType::AT_AND);
                    break;
                case 'O':
                    program.addType(cmd::AddType::AT_OR);
                    break;
                case 'F': {
                    double val = atof(optarg);
                    if(val >= 4)
                        program.forceFPS(val);
                    else {
                        std::cerr << "acidcam: Invalid FPS\n";
                        exit(0);
                    }
                }
                    break;
                case 'I': {
                    int skip_ = atoi(optarg);
                    if(skip_ > 1) {
                        program.skipFrames(skip_);
                    } else {
                        std::cerr << "acidcam: Requires to skip at least 2 frames\n";
                        exit(0);
                    }
                }
                    break;
                default:
                    std::cerr << "acidcam: Error incorrect input..\n";
                    exit(EXIT_FAILURE);
                    break;
            }
        }
    } else {
        output_software_info(argv[0]);
        exit(EXIT_FAILURE);
    }
    if(input.length()==0 || output.length()==0) {
        std::cerr << "acidcam: Invalid input/output use video files.\n";
        exit(EXIT_FAILURE);
    }
    
    if(filter_list.size()==0) {
        std::cout << "acidcam: You must select at least one filter with -f\n";
        exit(EXIT_FAILURE);
    }
    
    cmd::clearCursor();
    std::cout << "\n";

    try {
        if(secondVideoFile != "") {
            if(!program.setVideo(files_list)) {
                std::cerr << "acidcam: Error could not open vidoe file..\n";
                exit(EXIT_FAILURE);
            }
        }
        if(program.initProgram(ftype, visible, input, output,filter_list, col, color_m)) {
            
            program.setBrightness(bright_);
            program.setGamma(gamma_);
            program.setSaturation(sat_);
            program.run();
        } else {
            std::cerr << "acidcam: Start of program failed..\n";
            exit(EXIT_FAILURE);
        }
    }
    catch(std::exception &e) {
        std::cerr << "acidcam: Exception: " << e.what() << "\n";
    } catch(...) {
        std::cerr << "acidcam: Exception thrown...\n";
        exit(EXIT_FAILURE);
    }
    return 0;
}
