#!/bin/sh

cp tu.map /usr/share/texmf-texlive/fonts/map/dvips/
cp -R texmf /usr/local/share/
mktexlsr
updmap-sys --enable Map tu.map
