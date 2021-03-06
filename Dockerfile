# Dockerfile for building a cross developing environment for Qt
# targeting Windows. It also builds the application in the current
# directory. The application will be statically linked against
# Qt.
#
# To build the environment invoke
#  docker build -t qt .
# in the directory of this file. This creates a docker image
# called "qt". Note that it will take a while if you are building
# this image the first time. The contained application will also
# be compiled (in principle the last step could be done within
# the running container).
#
# Once build, you can enter the container via
#  docker run qt -ti bash
#
# (c) 2014,2015 by Sebastian Bauer
#
# Note that Docker requires a relatively recent Linux kernel.
# 3.8 is the current minimum.
#


FROM debian:buster

RUN apt-get update
RUN apt-get install -y --no-install-recommends \
	zip \
	autoconf \
	automake \
	autopoint \
	binutils \
	bison \
	build-essential \
	ca-certificates \
	cmake \
	debhelper \
	devscripts \
	fakeroot \
	flex \
	gcc \
	git \
	gperf \
	intltool \
	libgdk-pixbuf2.0-dev \
	libffi-dev \
	libgmp-dev \
	libmpc-dev \
	libmpfr-dev \
	libtool \
	libtool-bin \
	libz-dev \
	openssl \
	patch \
	pkg-config \
	p7zip-full \
	ruby \
	scons \
	subversion \
	texinfo \
	unzip \
	wget lzip liblz-dev libssl-dev openssl

# see http://stackoverflow.com/questions/10934683/how-do-i-configure-qt-for-cross-compilation-from-linux-to-windows-target

# Preapre and download cross development environment
RUN mkdir /build
WORKDIR  /build
RUN git clone https://github.com/mxe/mxe.git

# Build cross environment
RUN cd mxe && make -j MXE_VERBOSE=1 V=1 VERBOSE=1 MXE_TARGETS=i686-w64-mingw32.shared cmake
RUN cd mxe && make -j MXE_TARGETS=i686-w64-mingw32.shared qtbase
RUN cd mxe && make -j MXE_TARGETS=i686-w64-mingw32.shared qtmultimedia
RUN cd mxe && make -j MXE_TARGETS=i686-w64-mingw32.shared libzip
RUN cd mxe && make -j MXE_TARGETS=i686-w64-mingw32.shared yaml-cpp
RUN cd mxe && make -j MXE_TARGETS=i686-w64-mingw32.shared qtsvg
RUN cd mxe && make -j MXE_TARGETS=i686-w64-mingw32.shared file
RUN cd mxe && make -j MXE_TARGETS=i686-w64-mingw32.shared openssl
#COPY qtwebkit.diff /build/qtwebkit.diff
#RUN patch /build/mxe/src/qtwebkit.mk </build/qtwebkit.diff
#RUN cd mxe && make MXE_TARGETS=i686-w64-mingw32.shared qtwebkit
#RUN cd mxe && make MXE_TARGETS=i686-w64-mingw32.shared qtwebview

# TODO: Cleanup all unneeded stuff to make a slim image

# Enhance path
ENV PATH /build/mxe/usr/bin:$PATH

# Add a qmake alias
RUN ln -s /build/mxe/usr/bin/i686-w64-mingw32.shared-qmake-qt5 /build/mxe/usr/bin/qmake

##########################################################################
# Here the project specific workflow starts.
#
# Now copy the sources. They will become part of the image.
RUN mkdir /src
COPY . /src

# Switch to the source directory
#RUN cd mxe && make MXE_TARGETS=i686-w64-mingw32.shared openssl
WORKDIR /src

# Now build the project
RUN qmake qdigi.pro && make -j
#RUN cp -r /build/mxe/usr/i686-w64-mingw32.shared/qt5/bin/*.dll qdigi
#RUN cp -r /build/mxe/usr/i686-w64-mingw32.shared/qt5/plugins/* qdigi
#RUN cp -r /build/mxe/usr/i686-w64-mingw32.shared/bin/*.dll qdigi
RUN ./copy.sh
#RUN make
#RUN find / |grep qwindows.dll
#RUN find / |grep qt|grep .dll
#RUN find / |grep zip|grep dll
