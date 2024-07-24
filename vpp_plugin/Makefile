VPP_ROOT ?= ~/vpp
VPP_BUILD_ROOT ?= $(VPP_ROOT)/build-root
VPP_INSTALL_PATH ?= $(VPP_BUILD_ROOT)/install-vpp-native/vpp
PLUGIN_PATH ?= /usr/lib/x86_64-linux-gnu/vpp_plugins

CFLAGS = -I$(VPP_ROOT)/src \
         -I$(VPP_ROOT)/src/vlib \
         -I$(VPP_ROOT)/src/vnet \
         -I$(VPP_INSTALL_PATH)/include \
         -Wall -Werror -fPIC

LDFLAGS = -shared -Wl,-soname,libmy_plugin.so

all: my_plugin.so

my_plugin.o: my_plugin.c
	$(CC) $(CFLAGS) -c -o my_plugin.o my_plugin.c

my_plugin.so: my_plugin.o
	$(CC) $(LDFLAGS) -o my_plugin.so my_plugin.o

install: my_plugin.so
	sudo cp my_plugin.so $(PLUGIN_PATH)

clean:
	rm -f my_plugin.o my_plugin.so
