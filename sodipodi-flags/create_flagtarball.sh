# This script should be run one directory lower than the sodipodi-flags CVS
# directory
RELEASE=sodipodi-flags-1.5
FILES="sodipodi-flags/cc/*.svg sodipodi-flags/nations/*.svg \
    sodipodi-flags/Regional/*.svg sodipodi-flags/Organizations/*.svg \
    sodipodi-flags/historic/*.svg sodipodi-flags/license/*.* \
    sodipodi-flags/3x2/*.svg sodipodi-flags/4x3/*.svg \
    sodipodi-flags/COPYING sodipodi-flags/README \
    sodipodi-flags/CREDITS sodipodi-flags/generate.sh"

if [ -d sodipodi-flags/cc ]; then
    rm -fr sodipodi-flags/cc
fi

if [ -f sodipodi-flags/gen_cc.pl ]; then
    cd sodipodi-flags 
    perl ./gen_cc.pl
    cd ..
fi

tar -cvf ${RELEASE}.tar ${FILES}
zip ${RELEASE}.zip ${FILES}
