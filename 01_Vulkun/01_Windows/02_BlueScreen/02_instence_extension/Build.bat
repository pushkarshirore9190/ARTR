cls

cl.exe /c /EHsc VK.c

rc.exe VK.rc

link.exe VK.res VK.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

VK.exe






