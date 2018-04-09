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

#ifndef __ACIDCAM__H__
#define __ACIDCAM__H__
#include"ac.h"
#include<iostream>
#include<iomanip>
#include<vector>
#include<unistd.h>
#include<dlfcn.h>
#define APP_VERSION "v1.06"

namespace cmd {
    
    enum class File_Type { MOV, AVI };
    std::ostream &operator<<(std::ostream &out, const File_Type &type);
    void setCursorPos(int y, int x);
    void clearCursor();
    using plugin_filter = void (*)(cv::Mat &frame);
    
    extern std::string colorMaps[];
    
    class AC_Program {
    public:
        AC_Program();
        ~AC_Program();
        AC_Program(const AC_Program &) = delete;
        AC_Program(AC_Program &&) = delete;
        AC_Program &operator=(const AC_Program &) = delete;
        AC_Program &operator=(AC_Program &&) = delete;
        bool initProgram(const File_Type &ft, bool visible, const std::string &input, const std::string &output, std::vector<unsigned int> &filter_list, std::vector<unsigned int> &col, int color_map);
        void run();
        void stop();
        bool setVideo(const std::string &text);
        void setBrightness(int b);
        void setGamma(int g);
        void setSaturation(int s);
        void setColorMap(int m);
        bool loadPlugin(const std::string &s);
        void callPlugin(cv::Mat &frame);
        bool isPluginLoaded() const { return (library != nullptr); }
        std::string getInput() const { return input_file; }
        std::string getOutput() const { return output_file; }
    private:
        std::string input_file, output_file;
        cv::VideoCapture capture, video_capture;
        cv::VideoWriter writer;
        std::vector<unsigned int> filters;
        bool is_visible;
        bool active;
        File_Type file_type;
        void *library;
        plugin_filter plugin;
        unsigned int bright_, gamma_, sat_;
        int color_map;
        bool secondVideo;
        int second_w, second_h;
    };
}

#endif

