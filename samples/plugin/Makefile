CPPFLAGS=-nologo -I../.. -LD

all : plugin.plugin

clean :
    -del plugin.plugin
    -del plugin.exp
    -del plugin.lib
    -del plugin.obj

plugin.plugin : plugin.cpp
    cl $(CPPFLAGS) $** -link -out:plugin.plugin -def:plugin.def
