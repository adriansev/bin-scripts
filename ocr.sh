#!/bin/sh

# takes one parameter, the path to a pdf file to be processed.
# uses custom script 'pdf2tif' to generate the tif files,
# generates them at 300x300 dpi.
# drops them in our current directory
# then runs $progdir/tesseract on them, deleting the .raw
# and .map files that tesseract drops.

pdf2tiff $1

# edit this to point to wherever you've got your tesseract binary
progdir=/usr/bin/tesseract

for j in *.tif

    do
    x=`basename $j .tif`
    ${progdir}/tesseract ${j} ${x}
    rm ${x}.raw
    rm ${x}.map

#un-comment next line if you want to remove the .tif files when done.
# rm ${j}
done
