
sudo apt-get install -y libopencv-dev cmake git pkg-config libavcodec-dev libavformat-dev libswscale-dev python3-dev python-numpy libjpeg-dev libpng-dev libtiff-dev autoconf automake libtool wget nano

mkdir -p build
cd build
git clone https://github.com/lostjared/libacidcam.git
cd libacidcam
mkdir -p build
cd build
cmake ..
make -j4
sudo make install
cd ..

