# dotimg - represent image using dots

This program is given an image in the PNG format and represents it using
monochrome dots, based on the values of the pixels.

## Installation

Edit `config.mk` to match your local setup (dotimg is installed into the
`/usr/local` namespace by default).

Afterward, enter the following command to build and install dotimg (if
necessary as root):

    make clean install

## Running dotimg 

After successfully installing dotimg, you can run it:

    dotimg <png_in>
