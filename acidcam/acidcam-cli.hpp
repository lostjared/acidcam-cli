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
#ifndef __ACIDCAM__H__
#define __ACIDCAM__H__
#include"ac.h"
#include<iostream>
#include<iomanip>
#include<vector>
#include<algorithm>
#include<unistd.h>
#include<dlfcn.h>
#define APP_VERSION "v1.11.0"

namespace cmd {
    
    cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor);
    
    enum class File_Type { MOV, AVI };
    std::ostream &operator<<(std::ostream &out, const File_Type &type);
    void setCursorPos(int y, int x);
    void clearCursor();
    using plugin_filter = void (*)(cv::Mat &frame);
    
    extern std::string colorMaps[];
    
    class AC_VideoCapture {
    public:
        AC_VideoCapture() = default;
        void setVideo(std::string filename);
        bool getFrame(cv::Mat &frame);
        
        cv::VideoCapture capture;
        std::string name;
    };
    
    enum class AddType { AT_ADD, AT_ADD_SCALE, AT_XOR, AT_AND, AT_OR };

    class AC_Program {
    public:
        AC_Program();
        ~AC_Program();
        AC_Program(const AC_Program &) = delete;
        AC_Program(AC_Program &&) = delete;
        AC_Program &operator=(const AC_Program &) = delete;
        AC_Program &operator=(AC_Program &&) = delete;
        void setStretch(bool b);
        void setResolutionResize(int rw, int rh);
        bool initProgram(const File_Type &ft, bool visible, const std::string &input, const std::string &output, std::vector<std::pair<int,int>> &filter_list, std::vector<std::pair<int,int>> &col, int color_map);
        void setImageFilename(const std::string &img);
        void run();
        void stop();
        void setFlip(bool flip_);
        bool setVideo(std::vector<std::string> &filenames);
        void setBrightness(int b);
        void setGamma(int g);
        void setSaturation(int s);
        void setColorMap(int m);
        void addType(AddType at_type);
        bool loadPlugin(const std::string &s);
        void callPlugin(cv::Mat &frame);
        bool isPluginLoaded() const { return (library != nullptr); }
        std::string getInput() const { return input_file; }
        std::string getOutput() const { return output_file; }
        std::vector<AC_VideoCapture*> video_files;
        void forceFPS(double fval);
        void skipFrames(unsigned int number);
    private:
        std::string input_file, output_file, image_file_blend;
        cv::VideoCapture capture;
        cv::VideoWriter writer;
        std::vector<std::pair<int, int>> filters;
        bool is_visible;
        bool is_stretch;
        bool active;
        File_Type file_type;
        void *library;
        plugin_filter plugin;
        unsigned int bright_, gamma_, sat_;
        int color_map;
        bool secondVideo;
        int second_w, second_h;
        bool flip;
        bool res_resize;
        int res_w, res_h;
        AddType add_type;
        double fps_force;
        unsigned int skip_frames, skip_index;
    };
}

#endif

