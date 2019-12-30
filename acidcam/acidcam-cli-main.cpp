/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/lostsidedead
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
 BSD 2-Clause License
 
 Copyright (c) 2019, Jared Bruni
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
 -d color order index
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
 -R stretch resize
 -u reszie
 -T set number of threads for mulithreaded filters
 -J set fourcc manually
 -H use HEVC x265 (if your system has it)
 -V use AVC x264
 -j second layer video file input
 */
cmd::AC_Program program;
std::string secondVideoFile;
bool plugin_active = false;

void custom_filter(cv::Mat &frame) {}
void listPlugins(std::string path, std::vector<std::string> &files);

#if METACALL_ENABLED == 1
void init_metacall();
void *matrix_SetPixel(void *args[]);
void *matrix_GetPixel(void *args[]);
void *matrix_GetPixelInteger(void *args[]);
void *matrix_SetPixelBGR(void *args[]);
void *call_filter(void *args[]);

void init_metacall() {
    metacall_register("matrix_setpixel", matrix_SetPixel, METACALL_PTR,4, METACALL_PTR, METACALL_INT, METACALL_INT, METACALL_INT);
    metacall_register("matrix_setpixel_bgr", matrix_SetPixelBGR, METACALL_PTR,6, METACALL_PTR, METACALL_INT, METACALL_INT, METACALL_INT, METACALL_INT, METACALL_INT);
    
    metacall_register("matrix_getpixel", matrix_GetPixel, METACALL_ARRAY, 3, METACALL_PTR, METACALL_INT, METACALL_INT);
    metacall_register("matrix_getpixel_int", matrix_GetPixelInteger, METACALL_INT, 3, METACALL_PTR, METACALL_INT, METACALL_INT);
    
    metacall_register("call_filter", call_filter, METACALL_INT,2, METACALL_PTR, METACALL_INT);
}

void *matrix_SetPixel(void *args[]) {
    cv::Mat *type = (cv::Mat*)metacall_value_to_ptr(args[0]);
    int x = metacall_value_to_int(args[1]);
    int y = metacall_value_to_int(args[2]);
    int color_value = metacall_value_to_int(args[3]);
    const unsigned char *bgr = (const unsigned char*)&color_value;
    type->at<cv::Vec3b>(y, x) = cv::Vec3b(bgr[0], bgr[1], bgr[2]);
    return metacall_value_create_ptr((void*)type);
}

void *matrix_SetPixelBGR(void *args[]) {
    cv::Mat *type = (cv::Mat*)metacall_value_to_ptr(args[0]);
    int x = metacall_value_to_int(args[1]);
    int y = metacall_value_to_int(args[2]);
    int color_value[3];
    color_value[0] = metacall_value_to_int(args[3]);
    color_value[1] = metacall_value_to_int(args[4]);
    color_value[2] = metacall_value_to_int(args[5]);
    cv::Vec3b pixel(color_value[0], color_value[1], color_value[2]);
    type->at<cv::Vec3b>(y, x) = pixel;
    return metacall_value_create_ptr((void*)type);
}

void *matrix_GetPixel(void *args[]) {
    cv::Mat *type = (cv::Mat*)metacall_value_to_ptr(args[0]);
    int x = metacall_value_to_int(args[1]);
    int y = metacall_value_to_int(args[2]);
    cv::Vec3b color_v = type->at<cv::Vec3b>(y, x);
    void *array[] = {metacall_value_create_char(color_v[0]), metacall_value_create_char(color_v[1]),metacall_value_create_char(color_v[2])};
    return metacall_value_create_array((const void **)array, sizeof(array)/sizeof(array[0]));
}

void *matrix_GetPixelInteger(void *args[]) {
    cv::Mat *type = (cv::Mat*)metacall_value_to_ptr(args[0]);
    int x = metacall_value_to_int(args[1]);
    int y = metacall_value_to_int(args[2]);
    /*int col =  metacall_value_to_int(args[3]);*/
    unsigned int value = 0;
    unsigned char *buf = (unsigned char *)&value;
    cv::Vec3b val(buf[0], buf[1], buf[2]);
    type->at<cv::Vec3b>(y, x) = val;
    return metacall_value_create_int(0);
}

void *call_filter(void *args[]) {
    cv::Mat *type = (cv::Mat*)metacall_value_to_ptr(args[0]);
    int index = metacall_value_to_int(args[1]);
    ac::CallFilter(index, *type);
    return metacall_value_create_int(0);
}

#endif

void plugin_callback(cv::Mat &frame) {
#if METACALL_ENABLED == 1
    if (plugin_active == true) {
        void *ret = 0;
        static int value_set = 0;
        if(value_set == 0) {
            ret = metacall("array_size", frame.cols, frame.rows);
            long value = metacall_value_to_long(ret);
            if(value != 0) {
                std::cerr << "acidcam: array_size should return 0.\n";
                exit(EXIT_FAILURE);
            }
            metacall_value_destroy(ret);
            value_set = 1;
        }
        const enum metacall_value_id ids[] =
        {
            METACALL_PTR
        };
        ret = metacallt("filter", ids, (void*)&frame);
        metacall_value_destroy(ret);
        return;
    }
#endif
    if(program.isPluginLoaded() && plugin_active == false) {
        program.callPlugin(frame);
    }
}

template<typename F>
void getList(std::string args, std::vector<std::pair<int,int>> &v, F func);
void getStringList(std::string args, std::vector<std::string> &v);
bool parseRes(const std::string &text, int &fw, int &fh);

void listFilters(bool sorted = false) {
    std::cout << "List of Filters by Index\n";
    std::vector<std::string> filter_names;
    for(int i = 0; i < ac::draw_max-4; ++i)
        filter_names.push_back(ac::draw_strings[i]);
    
    if(sorted == true) sort(filter_names.begin(), filter_names.end());
    
    for(unsigned i = 0; i < filter_names.size(); ++i) {
        int index = ac::filter_map[filter_names[i]];
        std::cout << std::setw(6) << std::left << index << std::setw(50) << std::left << filter_names[i] << "\n";
    }
    std::cout << "\nList of Color Maps\n";
    for(int i = 1; i <= 12; ++i) {
        std::cout << std::setw(6) << std::left << i << std::setw(50) << std::left << cmd::colorMaps[i-1] << "\n";
    }
}

void toLower(std::string &text) {
    for(unsigned int i = 0; i < text.length(); ++i) {
        text[i] = tolower(text[i]);
    }
}

template<typename F>
void getList(std::string args, std::vector<std::pair<int,int>> &v, F func) {
    std::string number;
    unsigned int pos = 0;
    int subfilter_value = -1;

    while(pos < args.length()) {
        subfilter_value = -1;
        if(args[pos] != ',')
            number += args[pos];
        if(args[pos] == ',') {
            unsigned int value = 0;
            if(number == "plugin")
                value = ac::filter_map["Plugin"];
            else {
                if(number.find(":") == std::string::npos) {
                	value = atoi(number.c_str());
                    subfilter_value = -1;
                    if(ac::draw_strings[value].find("SubFilter") != std::string::npos) {
                        std::cerr << "acidcam: Error filter: " << ac::draw_strings[value] << " requires subfilter...\n";
                        exit(EXIT_FAILURE);
                    }
                } else {
                    std::string one1 = number.substr(0, number.find(":"));
                    std::string two2 = number.substr(number.find(":")+1, number.length());
                    value = atoi(one1.c_str());
                    subfilter_value = atoi(two2.c_str());
                    if(static_cast<int>(value) < ac::draw_max-4 && subfilter_value < ac::draw_max-4) {
                        if(ac::draw_strings[value].find("SubFilter") == std::string::npos) {
                            std::cerr << "acidcam: " << ac::draw_strings[value] << " does not take a SubFilter...\n";
                            exit(EXIT_FAILURE);
                        }
                        if(ac::draw_strings[subfilter_value].find("SubFilter") != std::string::npos) {
                            std::cerr << "acidcam: " << ac::draw_strings[subfilter_value] << " not a vaild subfilter...\n";
                            exit(EXIT_FAILURE);
                        }
                    } else {
                        std::cerr << "acidcam: Filter: " << one1 << " out of range...\n";
                        exit(EXIT_SUCCESS);
                    }
                }
            }
            number = "";
            if(func(value))
                v.push_back(std::make_pair(value,subfilter_value));
        }
        ++pos;
    }
    if(number.length() > 0) {
        subfilter_value = -1;
        unsigned int value = 0;
        if(number == "plugin")
            value = ac::filter_map["Plugin"];
        else
            value = atoi(number.c_str());
        
        if(number.find(":") != std::string::npos) {
            
            std::string one1 = number.substr(0, number.find(":"));
            std::string two2 = number.substr(number.find(":")+1, number.length());
            value = atoi(one1.c_str());
            subfilter_value = atoi(two2.c_str());
            if(static_cast<int>(value) < ac::draw_max-4 && subfilter_value < ac::draw_max-4) {
                if(ac::draw_strings[value].find("SubFilter") == std::string::npos) {
                    std::cerr << "acidcam: " << ac::draw_strings[value] << " does not take a SubFilter...\n";
                    exit(EXIT_FAILURE);
                }
                if(ac::draw_strings[subfilter_value].find("SubFilter") != std::string::npos) {
                    std::cerr << "acidcam: " << ac::draw_strings[subfilter_value] << " not a vaild subfilter...\n";
                    exit(EXIT_FAILURE);
                }
            } else {
                std::cerr << "acidcam: Filter: " << one1 << " out of range...\n";
                exit(EXIT_SUCCESS);
            }
        }
        
        if(func(value)) {
            v.push_back(std::make_pair(value, subfilter_value));
            subfilter_value = -1;
        }
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
            if((fullpath.rfind(".py") != std::string::npos) || (fullpath.rfind(".acf") != std::string::npos)) {
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
    std::cout << name << " " << APP_VERSION << " filters version: " << ac::version <<
    ((ac::OpenCL_Enabled()) ? "\nOpenCL Enabled." : "\nOpenCL Not Enabled") <<
    "\n\nCommand Line Arguments\n-l List filters\n-L list filters sorted by name\n-i input video\n-o output video\n-f filter list\n-v image visible\n-c r,g,b set colors\n-d set color index\n-p plugin\n-x list plugins in path\n-n plugin index\n-g image file for blend with image filters\n-b brightness\n-m gamma\n-s saturation\n-r colormap\n-k color key image\n-a additional videos\n-A add together frames for multiple video files.\n-C add together by scale of how many videos\n-O Use or to concat multiple videos\n-N use and to concat multiple video files.\n-X Xor frames for multiple video files.\n-e source flip video frame\n-S subfilter\n-u Resolution ex: 1920x1080\n-R stretch on resize\n-F force fps\n-I skip frames\n-T Set thread count (default is 4)\n-H use HEVC x265 (if your system has it)\n-V use AVC x264\n-J fourcc\n-j second layer video\n";
    
#if METACALL_ENABLED
    std::cout << "-P set python script path\n";
#endif
    
}

/* main function */
int main(int argc, char **argv) {
    ac::fill_filter_map();
    ac::setThreadCount(4);
    std::vector<std::string> files_list;
    ac::setPlugin(plugin_callback);
    std::string input,output;
    std::vector<std::pair<int, int>> filter_list;
    std::vector<std::pair<int, int>> col;
    bool visible = false;
    cmd::File_Type ftype;
    int bright_ = 0, gamma_ = 0, sat_ = 0, color_m = 0;
#if METACALL_ENABLED == 1
    metacall_log_stdio_type log_stdio = { stdout };
    metacall_log(METACALL_LOG_STDIO, (void *)&log_stdio);
    metacall_initialize();
    init_metacall();
#endif
    if(argc > 1) {
        int opt = 0;
        cmd::cur_codec = 0;
        while((opt = getopt(argc, argv, "Lli:o:f:vc:p:xn:hg:b:m:s:r:k:a:eS:u:CXANOF:I:RP:E:T:d:HAJ:j:")) != -1) {
            switch(opt) {
                case 'j': {
                    std::string fname = optarg;
                    ac::v_cap.open(fname);
                    if(!ac::v_cap.isOpened()) {
                        std::cerr << "acidcam: Error could not open video file: " << fname << "\n";
                        exit(EXIT_FAILURE);
                    }
                }
                    break;
                case 'H':
                    cmd::cur_codec = 2;
                    program.setCodecMode(2);
                    break;
                case 'V':
                    cmd::cur_codec = 2;
                    program.setCodecMode(1);
                    break;
                case 'J': {
                    std::string value = optarg;
                    if(value.length() != 4) {
                        std::cout << argv[0] << ": invalid fourcc...\n";
                        exit(EXIT_FAILURE);
                    }
                    cmd::cur_codec = 3;
                    cmd::four_cc = cv::VideoWriter::fourcc(value[0], value[1], value[2], value[3]);
                }
                    break;
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
                case 'R':
                    program.setStretch(true);
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
                    auto pos = output_l.find(".mp4");
                    auto pos_m = output_l.find(".mov");
                    auto pos_k = output_l.find(".mkv");
                    auto pos_w = output_l.find(".webp");
                    if(pos == std::string::npos && pos_m == std::string::npos && pos_k == std::string::npos && pos_w == std::string::npos) {
                        auto pos2 = output_l.find(".avi");
                        if(pos2 == std::string::npos) {
                        	output += ".mp4";
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
                        int subfilter_value = -1;
                        std::string number = optarg;
                        std::string plug = optarg;
                        if(plug == "plugin")
                            value = ac::filter_map["Plugin"];
                        else if(number.find(":") == std::string::npos) {
                            value = atoi(number.c_str());
                            if(static_cast<int>(value) < ac::draw_max-4) {
                                if(ac::draw_strings[value].find("SubFilter") != std::string::npos) {
                                    std::cout << "acidcam: Filter " << ac::draw_strings[value] << " requires SubFilter use Filter:SubFilter format\n";
                                    exit(EXIT_FAILURE);
                                }
                            }
                        } else {
                            value = atoi(optarg);
                            std::string one1 = number.substr(0, number.find(":"));
                            std::string two2 = number.substr(number.find(":")+1, number.length());
                            value = atoi(one1.c_str());
                            subfilter_value = atoi(two2.c_str());
                            if(static_cast<int>(value) < ac::draw_max-4 && subfilter_value < ac::draw_max-4) {
                                if(ac::draw_strings[value].find("SubFilter") == std::string::npos) {
                                    std::cerr << "acidcam: " << ac::draw_strings[value] << " does not take a SubFilter...\n";
                                    exit(EXIT_FAILURE);
                                }
                                if(ac::draw_strings[subfilter_value].find("SubFilter") != std::string::npos) {
                                    std::cerr << "acidcam: " << ac::draw_strings[subfilter_value] << " not a vaild subfilter...\n";
                                    exit(EXIT_FAILURE);
                                }
                            } else {
                                std::cerr << "acidcam: Filter: " << one1 << " out of range...\n";
                                exit(EXIT_SUCCESS);
                            }
                        }
                        if(static_cast<int>(value) < ac::draw_max-4) {
                            filter_list.push_back(std::make_pair(value, subfilter_value));
                        } else {
                            std::cerr << "acidcam: Error filter out of bounds..\n";
                        }
                    } else if(args.length() == 0) {
                        std::cerr << "acidcam: Error requires at least one filter.\n";
                        exit(EXIT_FAILURE);
                        
                    } else {
                        // list of filters
                        getList(args, filter_list, [](unsigned int value) {
                            if(static_cast<int>(value) < ac::draw_max-4)
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
                        if(static_cast<int>(value) <= 255)
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
#if METACALL_ENABLED == 1
                    if(program.loadPlugin(optarg)) {
                        std::cout << "acidcam: Loaded plugin: " << optarg << "\n";
                        init_metacall();
                        plugin_active = true;
                    } else {
                        std::cerr << "acidcam: Could not load plugin... exiting...\n";
                        plugin_active = false;
                        exit(EXIT_FAILURE);
                    }
#endif

                }
                    break;
                    
                case 'd': {
                    int color_value = atoi(optarg);
                    if(color_value >= 0 && color_value < 6) {
                        ac::color_order = color_value;
                    } else {
                        std::cerr << "acidcam: Invalid color order value\n";
                        exit(EXIT_FAILURE);
                    }
                }
                    break;
                    
                case 'x': {
#if METACALL_ENABLED == 1
                    std::vector<std::string> v;
                    listPlugins(".", v);
                    if(v.size() > 0) {
                        std::cout << "acidcam: Plugins found\n";
                        for(unsigned int i = 0; i < v.size(); ++i) {
                            std::cout << std::setw(4) << std::left << i << std::setw(50) << std::left << v[i] << "\n";
                        }
                        
                    } else {
                        std::cout << "acidcam: No plugins found\n";
                    }
                    exit(EXIT_SUCCESS);
#endif
                }
                    break;
                case 'E': {
#if METACALL_ENABLED == 1
                    std::vector<std::string> v;
                    listPlugins(".", v);
                    unsigned int plug = atoi(optarg);
                    if(v.size() > 0 && (plug >= 0 && plug < v.size())) {
                        if(program.loadPlugin(v[plug])) {
                            std::cout << "acidcam: Loaded plugin: " << v[plug] << "\n";
                            plugin_active = true;
                            init_metacall();
                        } else {
                            std::cerr << "acidcam: Error could not load plugin: " << v[plug] << "\n";
                            plugin_active = false;
                        }
                    }
#endif
                }

                    break;
                case 'n': {
                    std::vector<std::string> v;
                    listPlugins(".", v);
                    unsigned int plug = atoi(optarg);
                    if(v.size() > 0 && (plug >= 0 && plug < v.size())) {
                        if(program.loadPlugin(v[plug])) {
                            std::cout << "acidcam: Loaded plugin: " << v[plug] << "\n";
                        } else {
                            std::cerr << "acidcam: Error could not load plugin: " << v[plug] << "\n";
                        }
                    }
                }
                    break;
                case 'P': {
#if METACALL_ENABLED == 1
                    std::string path = optarg;
                    std::string lib_folder = path+"/lib";
                    std::string share_folder = path+"/share/metacall/configurations/global.json";
                    setenv("CONFIGURATION_PATH", share_folder.c_str(), 1);
                    setenv("LOADER_LIBRARY_PATH", lib_folder.c_str(), 1);
                    setenv("SERIAL_LIBRARY_PATH", lib_folder.c_str(), 1);
                    setenv("DETOUR_LIBRARY", lib_folder.c_str(), 1);
#endif
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
                    if(subf >= 0 && subf <= ac::draw_max-4) {
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
                case 'T': {
                    int num = atoi(optarg);
                    if(num >= 1)
                        ac::setThreadCount(num);
                    std::cout << "acidcam: Thread count set to: " << num << "\n";
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
#if METACALL_ENABLED == 1
            metacall_destroy();
#endif
            exit(EXIT_FAILURE);
        }
    }
    catch(std::exception &e) {
        std::cerr << "acidcam: Exception: " << e.what() << "\n";
    } catch(...) {
        std::cerr << "acidcam: Exception thrown...\n";
#if METACALL_ENABLED == 1
        metacall_destroy();
#endif
        exit(EXIT_FAILURE);
    }
    return 0;
}
