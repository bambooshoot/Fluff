set path=D:\asunlab\outSource\bin\openexr\lib;D:\asunlab\outSource\bin\ilmbase\lib;D:\asunlab\outSource\bin\zlib-1.2.11\bin
D:\asunlab\outsource\vcpkg-master\packages\openexr_x64-windows\tools\openexr\exrmultipart -combine -i fuzzy.exr::gradient clump.exr::density fuzzy.exr::length color.exr::colorTp color.exr::colorRt fuzzy.exr::split fuzzy.exr::clump -o layers.exr
pause