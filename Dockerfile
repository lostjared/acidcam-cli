FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y cmake git libgtk2.0-dev pkg-config libavcodec-dev \
libavformat-dev libswscale-dev python-dev python-numpy libtbb2 libtbb-dev \
libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev unzip autoconf automake libtool wget nano

RUN cd \
    && wget https://github.com/opencv/opencv/archive/3.2.0.zip \
    && unzip 3.2.0.zip \
    && cd opencv-3.2.0 \
    && mkdir build \
    && cd build \
    && cmake .. \
    && make -j8 \
    && make install \
    && cd \
    && rm 3.2.0.zip

RUN cd \
    && wget https://github.com/opencv/opencv_contrib/archive/3.2.0.zip \
    && unzip 3.2.0.zip \
    && cd opencv-3.2.0/build \
    && cmake -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.2.0/modules/ .. \
    && make -j8 \
    && make install \
    && cd ../.. \
    && rm 3.2.0.zip

RUN cd \
    && git clone https://github.com/lostjared/libacidcam.git \
    && cd libacidcam && mkdir -p build && cd build && cmake .. && make -j8 && make install \
    && cd .. && cd .. && rm -rf libacidcam
RUN cd \
    && git clone https://github.com/lostjared/acidcam-cli.git \
    && cd acidcam-cli && mkdir -p build && cd build && cmake .. && make -j8 && make install \
    && cd .. && cd .. &&  rm -rf acidcam-cli && ldconfig

RUN cd && apt-get autoremove -y && apt-get clean

CMD ['acidcam-cli']
