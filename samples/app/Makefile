CPPFLAGS=-nologo -I../.. -EHsc

all : app.exe

clean :
    -del app.exe
    -del app.obj
    -del lplugfwk.obj

app.exe : app.c ../../lplugfwk.cpp
    cl $(CPPFLAGS) $**
