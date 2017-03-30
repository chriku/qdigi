#ls /build/mxe/usr/i686-w64-mingw32.shared/qt5/bin/*.dll qdigi
#ls /build/mxe/usr/i686-w64-mingw32.shared/qt5/plugins/* qdigi
ls /build/mxe/usr/i686-w64-mingw32.shared/bin/*.dll qdigi
#cp -r /build/mxe/usr/i686-w64-mingw32.shared/qt5/bin/*.dll qdigi
cp -r /build/mxe/usr/i686-w64-mingw32.shared/qt5/plugins/platforms qdigi
cp -r /build/mxe/usr/i686-w64-mingw32.shared/qt5/plugins/imageformats qdigi
#cp -r /build/mxe/usr/i686-w64-mingw32.shared/bin/*.dll qdigi
cp -r release/qdigi.exe qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/qt5/bin/Qt5Core.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/qt5/bin/Qt5Gui.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/qt5/bin/Qt5Network.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/qt5/bin/Qt5Widgets.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libbz2.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libeay32.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libfreetype-6.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libgcc_s_sjlj-1.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libglib-2.0-0.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libharfbuzz-0.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libiconv-2.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libidn-11.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libintl-8.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libjpeg-9.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/liblzo2-2.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libnettle-6.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libpcre-1.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libpcre16-0.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libstdc++-6.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/libzip-4.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/ssleay32.dll qdigi
cp /build/mxe/usr/i686-w64-mingw32.shared/bin/zlib1.dll qdigi
rm -f qdigi.zip && zip -r qdigi.zip qdigi
