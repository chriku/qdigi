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


FROM debian:jessie

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
	wget

# see http://stackoverflow.com/questions/10934683/how-do-i-configure-qt-for-cross-compilation-from-linux-to-windows-target

# Preapre and download cross development environment
RUN mkdir /build
WORKDIR  /build
RUN git clone https://github.com/mxe/mxe.git

# Build cross environment
RUN cd mxe && make MXE_TARGETS=i686-w64-mingw32.shared qtbase
RUN cd mxe && make MXE_TARGETS=i686-w64-mingw32.shared qtmultimedia
RUN cd mxe && make MXE_TARGETS=i686-w64-mingw32.shared libzip

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
WORKDIR /src

# Now build the project
RUN qmake qdigi.pro && make -j 8
RUN mkdir -p outDir
RUN cp -r /build/mxe/usr/i686-w64-mingw32.shared/bin/*.dll outDir
RUN cp -r release/qdigi.exe outDir
RUN zip out.zip outDir
#RUN make
#RUN find / |grep libzip-4.dll
