ktxtool
========

A conversion and compression tool for the KTX image format. See more here http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/

It supports PPM input format without any external dependency. But also TIFF, PNG and JPG as a standalone implementations. It also has an ImageMagick implementation (that suppports all major file formats).

Building
-------------
If you want to use imagemagick set the cmake option WITH-IMAGEMAGICK to true like this:

cmake ./ -DWITH-IMAGEMAGICK=true

Be aware that this will disable all other "WITH" options as imagemagick already implement those formats and they're not needed.

To enable multithreading set WITH-TBB to true like this:

cmake ./ -DWITH-TBB=true

With this the compression is reduced by the number of your CPU cores. Is recommended to have this enabled but it's not required.

Usage
----------------

To generate a cube map texture type this command
ktxtool face1.jpg,face2.jpg,face3.jpg,face4.jpg,face5.jpg,face6.jpg

Current State
-------------
Only RGB8 and RGBA8 is supported either raw or compressed, as for compression goes only ETC1 is implemented. I made this tool for mobile development so even if this tool is far from complete it could be used for production already if your usage match mine.

TODO
--------
PNG and JPG input formats are expected before v0.2.0

S3TC compression is expected before v0.2.0

