#!/bin/sh

cp tu.map /usr/share/texmf-texlive/fonts/map/dvips/
cp -R texmf nach /usr/local/share/
mktexlsr
updmap-sys --enable Map tu.map
