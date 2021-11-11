CPPFLAGS=-nologo -I../.. -EHsc

all : app.exe

clean :
    -del app.exe
    -del app.obj
    -del lplugfwk.obj

app.exe : app.cpp ../../lplugfwk.cpp
    cl $(CPPFLAGS) $**
