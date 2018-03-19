./configure \
 --cc="gcc -lstdc++" \
 --extra-libs="-lboost_filesystem -lmyfunc -lwrapperndn" \
 --enable-shared \
 --enable-pic \
 --pkg-config-flags=--static \
 --enable-libx264 \
 --enable-gpl
