sudo apt-get -y install libglfw3
sudo apt-get -y install libglfw3-dev
sudo apt-get -y install cmake
sudo apt-get -y install libfreetype6-dev
sudo apt-get -y install g++
mkdir snowsim
sudo mount -t tmpfs -o size=8000m tmpfs snowsim
cd snowsim
git clone https://github.com/changyoonpark/mpm.git
cd mpm
rm -rf build
mkdir build
cd build
mkdir outputs
cmake ..
make