#############################################################################
# Makefile for building: sweep.app/Contents/MacOS/sweep
# Generated by qmake (1.07a) (Qt 3.3.8) on: Wed Sep 19 16:12:30 2007
# Project:  sweep.pro
# Template: app
# Command: $(QMAKE) -o makefile.sw sweep.pro
#############################################################################

####### Compiler, tools and options

CC       = cc
CXX      = c++
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -Wall -W -g  -DQT_THREAD_SUPPORT -DQT_SHARED -DQT_ACCESSIBILITY_SUPPORT -DQT_TABLET_SUPPORT -DQT_NO_DEBUG
CXXFLAGS = -pipe -Wall -W -g  -DQT_THREAD_SUPPORT -DQT_SHARED -DQT_ACCESSIBILITY_SUPPORT -DQT_TABLET_SUPPORT -DQT_NO_DEBUG
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I/opt/local/share/qt3/mkspecs/default -I. -I../../include -I$(HOME)/fs/include -I/opt/local/include -I$(QTDIR)/include/qt3
LINK     = c++
LFLAGS   = -headerpad_max_install_names -prebind
LIBS     = $(SUBLIBS) -L$(QTDIR)/lib -lAOS3 -lAOS3_core -lCGALQt -lCGAL -L../../src -L../../lib_accel -L$(HOME)/fs/lib -lgmp -lmpfr -lqt-mt
AR       = ar cq
RANLIB   = ranlib -s
MOC      = $(QTDIR)/bin/moc
UIC      = $(QTDIR)/bin/uic
QMAKE    = qmake
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= cp -f
COPY_DIR = cp -f -r
INSTALL_FILE= $(COPY_FILE)
INSTALL_DIR = $(COPY_DIR)
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = ./

####### Files

HEADERS = 
SOURCES = sweep.cpp
OBJECTS = sweep.o
FORMS = 
UICDECLS = 
UICIMPLS = 
SRCMOC   = 
OBJMOC = 
DIST	   = sweep.pro
QMAKE_TARGET = sweep
DESTDIR  = sweep.app/Contents/MacOS/
TARGET   = sweep.app/Contents/MacOS/sweep

first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: makefile.sw sweep.app/Contents/MacOS/../PkgInfo sweep.app/Contents/MacOS/../Info.plist $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	test -d sweep.app/Contents/MacOS/ || mkdir -p sweep.app/Contents/MacOS/
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(OBJCOMP) $(LIBS)

mocables: $(SRCMOC)
uicables: $(UICDECLS) $(UICIMPLS)

$(MOC): 
	( cd $(QTDIR)/src/moc && $(MAKE) )

makefile.sw: sweep.pro  /opt/local/share/qt3/mkspecs/default/qmake.conf /opt/local/lib/libqt-mt.prl
	$(QMAKE) -o makefile.sw sweep.pro
qmake: 
	@$(QMAKE) -o makefile.sw sweep.pro

sweep.app/Contents/MacOS/../PkgInfo: 
	@test -d sweep.app/Contents/MacOS/ || mkdir -p sweep.app/Contents/MacOS/
	@$(DEL_FILE) sweep.app/Contents/MacOS/../PkgInfo
	@echo "APPL????" >sweep.app/Contents/MacOS/../PkgInfo
sweep.app/Contents/MacOS/../Info.plist: 
	@test -d sweep.app/Contents/MacOS/ || mkdir -p sweep.app/Contents/MacOS/
	@$(DEL_FILE) sweep.app/Contents/MacOS/../Info.plist
	@sed -e "s,@ICON@,application.icns,g" -e "s,@EXECUTABLE@,sweep,g" "/opt/local/share/qt3/mkspecs/default/Info.plist.app" >"sweep.app/Contents/MacOS/../Info.plist"
dist: 
	@mkdir -p .tmp/sweep && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) .tmp/sweep/ && ( cd `dirname .tmp/sweep` && $(TAR) sweep.tar sweep && $(GZIP) sweep.tar ) && $(MOVE) `dirname .tmp/sweep`/sweep.tar.gz . && $(DEL_FILE) -r .tmp/sweep

mocclean:

uiclean:

yaccclean:
lexclean:
clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) -r sweep.app


FORCE:

####### Compile

sweep.o: sweep.cpp 

####### Install

install:  

uninstall:  

