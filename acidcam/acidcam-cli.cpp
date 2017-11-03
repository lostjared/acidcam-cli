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
#include<unistd.h>
#include<cstdlib>
#include<cstring>
#include<cctype>
/* required to be declared in source file */
/*
 Command Line Arguments
 -l List filters
 -i input video
 -o output video
 -f filter list
 -v image visible
 */
cmd::AC_Program program;
cv::Mat blend_image;
bool blend_set = false;
void custom_filter(cv::Mat &frame) {}
void ac::plugin(cv::Mat &frame) {}

template<typename F>
void getList(std::string args, std::vector<int> &v, F func);

void listFilters() {
    std::cout << "List of Filters by Index\n";
    for(unsigned int i = 0; i < ac::draw_max-3; ++i) {
        std::cout << std::setw(4) << std::left << i << std::setw(50) << std::left << ac::draw_strings[i] << "\n";
    }
}

void toLower(std::string &text) {
    for(unsigned int i = 0; i < text.length(); ++i) {
        text[i] = tolower(text[i]);
    }
}

template<typename F>
void getList(std::string args, std::vector<int> &v, F func) {
    std::string number;
    unsigned int pos = 0;
    while(pos < args.length()) {
        if(args[pos] != ',')
            number += args[pos];
        if(args[pos] == ',') {
            unsigned int value = atoi(number.c_str());
            number = "";
            if(func(value))
            	v.push_back(value);
        }
        ++pos;
    }
    if(number.length() > 0) {
        unsigned int value = atoi(number.c_str());
        if(func(value))
        	v.push_back(value);
    }
}

void control_Handler(int sig) {
    std::cout << "\nacidcam: Signal caught stopping...\n";
    program.stop();
}

/* main function */
int main(int argc, char **argv) {
    ac::fill_filter_map();
    std::string input,output;
    std::vector<int> filter_list;
    std::vector<int> col;
    bool visible = false;
    cmd::File_Type ftype;
    
    if(argc > 1) {
        int opt = 0;
        while((opt = getopt(argc, argv, "li:o:f:vc:")) != -1) {
            switch(opt) {
                case 'l':
                    listFilters();
                    exit(EXIT_SUCCESS);
                    break;
                case 'i':
                    input = optarg;
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
                        unsigned int value = atoi(optarg);
                        if(value >= 0 && value <= ac::draw_max-5) {
                            filter_list.push_back(value);
                        } else {
                            std::cerr << "acidcam: Error filter out of bounds..\n";
                        }
                    } else if(args.length() == 0) {
                        std::cerr << "acidcam: Error requires at least one filter.\n";
                        exit(EXIT_FAILURE);
                        
                    } else {
                        // list of filters
                        getList(args, filter_list, [](int value) {
                            if(value >= 0 && value < ac::draw_max-5)
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
                    getList(colors, col, [](int value) {
                        if(value >= 0 && value <= 255)
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
                default:
                    std::cerr << "acidcam: Error incorrect input..\n";
                    exit(EXIT_FAILURE);
                    break;
            }
        }
    } else {
        std::cout << "acidcam [ -l list -i input -o output ]\n";
        exit(EXIT_FAILURE);
    }
    if(input.length()==0 || output.length()==0) {
        std::cerr << "acidcam: Invalid input/output use video files.\n";
        exit(EXIT_FAILURE);
    }
    
    try {
        if(program.initProgram(ftype, visible, input, output,filter_list, col)) {
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
