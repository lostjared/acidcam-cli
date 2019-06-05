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
#include<sys/stat.h>
#include<sys/types.h>
#include<signal.h>
#include<cmath>

extern void control_Handler(int sig);

namespace cmd {
    
    using namespace cv;
    
    //  Function below from Stack Overflow
    // https://stackoverflow.com/questions/28562401/resize-an-image-to-a-square-but-keep-aspect-ratio-c-opencv
    cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor) {
        cv::Mat output;
        double h1 = dstSize.width * (input.rows/(double)input.cols);
        double w2 = dstSize.height * (input.cols/(double)input.rows);
        if( h1 <= dstSize.height) {
            cv::resize( input, output, cv::Size(dstSize.width, h1));
        } else {
            cv::resize( input, output, cv::Size(w2, dstSize.height));
        }
        int top = (dstSize.height-output.rows) / 2;
        int down = (dstSize.height-output.rows+1) / 2;
        int left = (dstSize.width - output.cols) / 2;
        int right = (dstSize.width - output.cols+1) / 2;
        cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor );
        return output;
    }
    
    
    // Resize X variable
    inline int AC_GetFX(int oldw,int x, int nw) {
        float xp = (float)x * (float)oldw / (float)nw;
        return (int)xp;
    }
    // Resize Y Variable
    inline int AC_GetFZ(int oldh, int y, int nh) {
        float yp = (float)y * (float)oldh / (float)nh;
        return (int)yp;
    }
    
    std::string colorMaps[] = {"Autum", "Bone", "Jet", "Winter", "Rainbow", "Ocean","Summer","Spring","Cool", "HSV", "Pink", "Hot"};
    
    void setCursorPos(int y, int x) {
        std::cout << "\033[" << (y+1) << ";" << (x+1) << "H";
    }
    
    void clearCursor() {
        std::cout << "\033[H\033[J";
    }
    
    std::ostream &operator<<(std::ostream &out, const File_Type &type) {
        if(type == File_Type::MOV)
            out << "MPEG-4 (Quicktime)";
        else
            out << "XviD";
        return out;
    }
    
    void AC_VideoCapture::setVideo(std::string filename) {
        if(filename.length() > 0) {
            capture.open(filename);
            if(!capture.isOpened()) {
                std::cerr << "acidcam: Error opening video: " << filename << "\n";
                exit(EXIT_FAILURE);
            }
            name = filename;
        }
    }
    
    bool AC_VideoCapture::getFrame(cv::Mat &frame) {
        if(capture.read(frame) == true) return true;
        return false;
    }
    
    AC_Program::AC_Program() {
        library = nullptr;
        bright_ = gamma_ = sat_ = 0;
        color_map = 0;
        secondVideo = false;
        second_w = 0;
        second_h = 0;
        flip = false;
        res_resize = false;
        res_w = 0;
        res_h = 0;
        add_type = AddType::AT_ADD;
        fps_force = 0;
        skip_frames = 0;
        skip_index = 0;
        is_stretch = false;
    }
    
    AC_Program::~AC_Program() {
        if(library != nullptr)
            dlclose(library);
        
        if(video_files.size() > 0) {
            for(unsigned int i = 0; i < video_files.size(); ++i)
                delete video_files[i];
            
            if(!video_files.empty())
                video_files.erase(video_files.begin(), video_files.end());
        }
    }
    
    void AC_Program::setStretch(bool b) {
        is_stretch = b;
    }
    
    bool AC_Program::loadPlugin(const std::string &s) {
        if(s.find(".py") != std::string::npos) {
#if METACALL_ENABLED == 1
            std::string dir = s;
            std::string path = dir.substr(0, dir.rfind("/"));
            std::string script_name=s.substr(s.rfind("/")+1, s.length());
            std::cout << "Attempting to load: " << script_name << "\n";
            const char * py_scripts[] = {
                script_name.c_str(),
            };
        if (metacall_load_from_file("py", py_scripts, sizeof(py_scripts) / sizeof(py_scripts[0]), NULL) == 0) {
            return true;
        }
#else
            std::cerr << "Error Python Not Enabled...\n";
            exit(EXIT_FAILURE);
#endif
        } else {
            library = dlopen(s.c_str(), RTLD_LAZY);
            if(library == NULL)
                return false;
            
            void *addr = dlsym(library, "filter");
            const char *err = dlerror();
            if(err) {
                std::cerr << "Could not locate function: filter in " << s << "\n";
                return false;
            }
            plugin = reinterpret_cast<plugin_filter>(addr);
        }
        return false;
    }
    
    void AC_Program::setFlip(bool f) {
        flip = f;
    }
    
    void AC_Program::addType(AddType at_type) {
        add_type = at_type;
    }
    
    void AC_Program::callPlugin(cv::Mat &frame) {
        if(library != nullptr)
            plugin(frame);
    }
    
    void AC_Program::setImageFilename(const std::string &img) {
        image_file_blend = img;
    }
    
    bool AC_Program::initProgram(const File_Type &ftype, bool visible, const std::string &input, const std::string &output, std::vector<std::pair<int,int>> &filter_list,std::vector<std::pair<int, int>> &col, int c_map) {
        color_map = c_map;
        file_type = ftype;
        is_visible = visible;
        input_file = input;
        output_file = output;
        filters = filter_list;
        capture.open(input_file);
        if(!capture.isOpened()) {
            std::cerr << "acidcam: Error could not open file: " << input_file << "\n";
            return false;
        }
        int aw = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
        int ah = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
        
        if(res_resize == true) {
            aw = res_w;
            ah = res_h;
        }
        
        double fps = capture.get(cv::CAP_PROP_FPS);
        fps = floorf(fps * 100) / 100;
        ac::fps = fps;
        second_w = aw;
        second_h = ah;
        if(fps < 1) {
            std::cerr << "acidcam: Invalid frame rate...\n";
            exit(EXIT_FAILURE);
        }
        
        if(fps_force != 0)
            fps = fps_force;
        
        if(file_type == File_Type::MOV)
            writer.open(output_file, VideoWriter::fourcc('m', 'p', '4', 'v'), fps, cv::Size(aw, ah), true);
        else
            writer.open(output_file, VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, cv::Size(aw, ah), true);
        if(!writer.isOpened()) {
            std::cerr << "acidcam: Error could not open file for writing: " << output_file << "\n";
            return false;
        }
        unsigned int num_frames = capture.get(cv::CAP_PROP_FRAME_COUNT);
        
        std::string img_str;
        if(image_file_blend.length() > 0) {
            img_str = " image[" + image_file_blend + "]";
        }
        
        std::ostringstream substream;
        if(ac::subfilter != -1)
            substream << "subfilter[" << ac::draw_strings[ac::subfilter] << "] ";
        
        std::string force_;
        if(fps_force != 0)
            force_ = "force";
        std::string skip_f;
        
        std::ostringstream skip_stream;
        if(skip_frames != 0)
            skip_stream << "skip frames: [" << skip_frames << "] ";
        
        double frame_length = num_frames/fps;
        
        if(fps_force != 0)
            frame_length = (num_frames/fps_force);
        frame_length = floorf(frame_length * 100) / 100;
        std::cout << "acidcam: input[" << input_file  << ((flip == true) ? " [flipped]" : "") << "] output[" << output_file << "] width[" << aw << "] height[" << ah << "] " << force_ << " fps[" << fps << "] length[" << frame_length << " seconds] " << skip_stream.str() << substream.str() << "format[" << file_type << "] " << img_str << "\n";
        
        if(video_files.size() > 0) {
            std::string add_type_str = "ADD";
            switch(add_type) {
                case AddType::AT_ADD:
                    add_type_str = "ADD";
                    break;
                case AddType::AT_ADD_SCALE:
                    add_type_str = "ADD SCALE";
                    break;
                case AddType::AT_OR:
                    add_type_str = "OR";
                    break;
                case AddType::AT_AND:
                    add_type_str = "AND";
                    break;
                case AddType::AT_XOR:
                    add_type_str = "XOR";
                    break;
            }
            std::cout << "\nAdditional Videos: [" << add_type_str << "] \n";
            for(unsigned int q = 0; q < video_files.size(); ++q)
                std::cout << video_files[q]->name << "\n";
        }
        std::cout << "\nFilters to Apply: \n";
        for(unsigned int q = 0; q < filter_list.size(); ++q) {
            std::cout << ac::draw_strings[filter_list[q].first];
            if(filter_list[q].second != -1)
                std::cout << " SubFilter: " << ac::draw_strings[filter_list[q].second] << "\n";
            else
                std::cout << "\n";
        }
        if(color_map >= 1 && color_map <= 12) {
            std::cout << "\nApplied Color Map: " << colorMaps[color_map-1] << "\n";
        }
        std::cout << "\n";
        if(col.size()==3) {
            ac::swapColor_b = static_cast<unsigned char>(col[2].first);
            ac::swapColor_g = static_cast<unsigned char>(col[1].first);
            ac::swapColor_r = static_cast<unsigned char>(col[0].first);
            std::cout << "Add RGB {" << col[0].first << ", " << col[1].first << ", " << col[2].first << "}\n";
        }
        bright_ = gamma_ = sat_ = 0;
        return true;
    }
    
    void AC_Program::stop() {
        active = false;
        setCursorPos(filters.size()+2+video_files.size()+4, 0);
    }
    
    bool AC_Program::setVideo(std::vector<std::string> &v) {
        for(unsigned int i = 0; i < v.size(); ++i) {
            AC_VideoCapture *cap = new AC_VideoCapture();
            cap->setVideo(v[i]);
            video_files.push_back(cap);
        }
        return true;
    }
    
    void AC_Program::setBrightness(int b) {
        bright_ = b;
    }
    
    void AC_Program::setGamma(int g) {
        gamma_ = g;
    }
    
    void AC_Program::setSaturation(int s) {
        sat_ = s;
    }
    
    void AC_Program::setColorMap(int m) {
        color_map = m;
    }
    
    void AC_Program::run() {
        struct stat buf;
        unsigned long frame_count_len = 0, frame_index = 0;
        unsigned int percent_now = 0;
        try {
            bool copy_orig = false;
            if(std::find(filters.begin(), filters.end(), std::make_pair(ac::filter_map["Blend with Source"], -1)) != filters.end()) {
                copy_orig = true;
            }
            if(colorkey_set == true && !color_image.empty()) {
                copy_orig = true;
            }
            frame_count_len = capture.get(cv::CAP_PROP_FRAME_COUNT);
            struct sigaction sa;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = control_Handler;
            if(sigaction(SIGINT, &sa, 0) == -1) {
                std::cerr << "Error on sigaction:\n";
                exit(EXIT_FAILURE);
            }
            if(is_visible)
                cv::namedWindow("acidcam_cli");
            active = true;
            
            setCursorPos(7+video_files.size()+2+filters.size(), 0);
            std::cout << "acidcam: Working frame: [0/" << frame_count_len << "] - 0% Size: 0 MB \n";
            while(active == true) {
                cv::Mat frame, temp_frame;
                if(capture.read(frame) == false) {
                    break;
                }
                
                if(res_resize == true) {
                    if(is_stretch) {
                        cv::Mat copy1 = frame.clone();
                        cv::resize(copy1, frame, cv::Size(res_w, res_h));
                    } else {
                        frame = resizeKeepAspectRatio(frame, cv::Size(res_w, res_h), cv::Scalar(0, 0, 0));
                    }
                }
                
                if(flip == true) {
                    cv::flip(frame, temp_frame, 1);
                    frame = temp_frame;
                }
                if(flip == true) {
                    cv::flip(frame, temp_frame, 0);
                    frame = temp_frame;
                }
                cv::Mat frame2;
                double fade_amount = 1.0;
                if(video_files.size()>0)
                    fade_amount = 1.0/1+video_files.size();
                
                for(unsigned int q = 0; q < video_files.size(); ++q) {
                    if(video_files[q]->capture.isOpened() && video_files[q]->capture.read(frame2) == true) {
                        for(unsigned int z = 0; z < frame.rows; ++z) {
                            for(unsigned int i = 0; i < frame.cols; ++i) {
                                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                                int cX = AC_GetFX(frame2.cols, i, frame.cols);
                                int cY = AC_GetFZ(frame2.rows, z, frame.rows);
                                cv::Vec3b second_pixel = frame2.at<cv::Vec3b>(cY, cX);
                                for(unsigned int j = 0; j < 3; ++j) {
                                    switch(add_type) {
                                        case AddType::AT_ADD:
                                            pixel[j] += second_pixel[j];
                                            break;
                                        case AddType::AT_ADD_SCALE:
                                            pixel[j] += static_cast<unsigned char>(second_pixel[j]*fade_amount);
                                            break;
                                        case AddType::AT_XOR:
                                            pixel[j] = pixel[j]^second_pixel[j];
                                            break;
                                        case AddType::AT_AND:
                                            pixel[j] = pixel[j]&second_pixel[j];
                                            break;
                                        case AddType::AT_OR:
                                            pixel[j] = pixel[j]|second_pixel[j];
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
                if(copy_orig == true) ac::orig_frame = frame.clone();
                frame_index ++;
                if(frame_index >= frame_count_len) {
                    break;
                }
                for(unsigned int i = 0; i < filters.size(); ++i) {
                    ac::setSubFilter(filters[i].second);
                    ac::CallFilter(filters[i].first, frame);
                }
                
                if(color_map != 0 && color_map > 0 && color_map <= 12) {
                    ac::set_color_map = color_map;
                    ac::ApplyColorMap(frame);
                }
                
                if(bright_ > 0)
                    ac::setBrightness(frame, 1.0, bright_);
                
                if(gamma_ > 0) {
                    cv::Mat cur = frame.clone();
                    ac::setGamma(cur,frame,gamma_);
                }
                if(sat_ > 0)
                    ac::setSaturation(frame, sat_);
                
                static cv::Vec3b color_key(255, 0, 255);
                if(colorkey_set == true && !color_image.empty()) {
                    cv::Mat cframe = frame.clone();
                    ac::filterColorKeyed(color_key, ac::orig_frame, cframe, frame);
                }
                if(skip_frames == 0)
                    writer.write(frame);
                else {
                    if((frame_index%skip_frames)==0) {
                        ++skip_index;
                        writer.write(frame);
                    }
                }
                double val = frame_index;
                double size = frame_count_len;
                if(size != 0) {
                    double percent = (val/size)*100;
                    unsigned int percent_trunc = static_cast<unsigned int>(percent);
                    unsigned int frame_fps = static_cast<unsigned int>(ac::fps);
                    if((percent_trunc > percent_now) || ((frame_index%frame_fps)==0) ) {
                        percent_now = percent_trunc;
                        
                        lstat(output_file.c_str(), &buf);
                        setCursorPos(7+video_files.size()+2+filters.size(), 0);
                        double sec_fps = frame_index/ac::fps;
                        unsigned int seconds_ = static_cast<unsigned int>(sec_fps);
                        if(skip_frames != 0) {
                            sec_fps = skip_index/ac::fps;
                            seconds_ = static_cast<unsigned int>(sec_fps);
                        }
                        if(fps_force != 0) {
                            sec_fps = frame_index/fps_force;
                            seconds_ = static_cast<unsigned int>(sec_fps);
                        }
                        std::cout << "acidcam: Working frame: [" << frame_index << "/" << frame_count_len << "] - " << percent_trunc << "% Size: " << ((buf.st_size/1024)/1024) << " MB - " << seconds_ << " Seconds\n";
                    }
                }
                if(is_visible) {
                    cv::imshow("acidcam_cli", frame);
                    int key = cv::waitKey(25);
                    if(key == 27) break;
                }
            }
        } catch(...) {
            writer.release();
            std::cerr << "acidcam: Error exception occoured..\n";
        }
        if(percent_now == 99) percent_now = 100;
        setCursorPos(7+video_files.size()+2+filters.size(), 0);
        std::cout << "acidcam: Done wrote to file [" << output_file << "] format[" << file_type << "] Size: " << ((buf.st_size/1024)/1024) << " MB\n";
    }
    
    void AC_Program::setResolutionResize(int rw, int rh) {
        res_resize = true;
        res_w = rw;
        res_h = rh;
    }
    
    void AC_Program::forceFPS(double fval) {
        fps_force = fval;
    }
    
    void AC_Program::skipFrames(unsigned int number) {
        skip_frames = number;
        skip_index = 0;
    }
}

