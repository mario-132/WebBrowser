echo "Now downloading and installing needed libraries"
mkdir ext
cd ext

git clone git://git.netsurf-browser.org/libwapcaplet.git
git clone git://git.netsurf-browser.org/libparserutils.git
#git clone git://git.netsurf-browser.org/libcss.git
git clone https://github.com/pcwalton/netsurf-buildsystem

cd netsurf-buildsystem
make
sudo make install

cd ..

export LIBRARY_PATH=/opt/netsurf
export C_INCLUDE_PATH=/opt/netsurf
export ECHO=echo
export GREP=grep
export PERL=perl
export PKGCONFIG=pkg-config
export BUILD_CC=gcc

cd libwapcaplet
make
sudo make install

cd ..

cd libparserutils
make
sudo make install

cd ..

cd libcss
make
sudo make install
