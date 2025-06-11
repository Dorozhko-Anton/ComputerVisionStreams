PLUGIN_NAME = recordplugin

CXX:= g++
SRCS:= gstrecordplugin.cpp
INCS:= gstrecordplugin.h
LIB:=libnvdsgst_recordplugin.so

TARGET_DEVICE = $(shell gcc -dumpmachine | cut -f1 -d -)

NVDS_VERSION:=7.0

GST_INSTALL_DIR?=/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/lib/gst-plugins/
LIB_INSTALL_DIR?=/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/lib/


CFLAGS+= -fPIC -DDS_VERSION=\"7.0.0\" \
	 -I/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/includes \
	 -I/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/libs \
	 -I/opt/nvidia/deepstream/deepstream-$(NVDS_VERSION)/libs/recordplugin/

LIBS := -shared -Wl,-no-undefined
LIBS+= -L$(LIB_INSTALL_DIR) -lnvds_meta -lnvdsgst_meta -ldl \
       -Wl,-rpath,$(LIB_INSTALL_DIR)

OBJS:= $(SRCS:.cpp=.o)

PKGS:= gstreamer-1.0 gstreamer-base-1.0 gstreamer-app-1.0
CFLAGS+= $(shell pkg-config --cflags $(PKGS))
LIBS+= $(shell pkg-config --libs $(PKGS))

all: $(LIB)

%.o: %.cpp $(INCS) Makefile
	@echo $(CFLAGS)
	$(CXX) -c -o $@ $(CFLAGS) $<

$(LIB): $(OBJS) $(DEP) Makefile
	$(CXX) -o $@ $(OBJS) $(LIBS)

install: $(LIB)
	cp -rv $(LIB) $(GST_INSTALL_DIR)

clean:
	rm -rf $(OBJS) $(LIB)