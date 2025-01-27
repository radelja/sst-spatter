#!/bin/bash

if [ -z $LIBTOOLIZE ] ; then
    LIBTOOLIZE=$(type -P libtoolize)
    if [ -z $LIBTOOLIZE ] ; then
        LIBTOOLIZE=$(type -P glibtoolize)
    fi
fi
if [ -z $LIBTOOL ] ; then
    LIBTOOL=$(type -P "${LIBTOOLIZE%???}")
fi

if [ -z $LIBTOOL ] || [ -z $LIBTOOLIZE ] ; then
    echo "Unable to find working libtool. [$LIBTOOL][$LIBTOOLIZE]"
    exit 1
fi

echo "Generating configure files..."
# Delete the old libtool output
rm -rf libltdl src/libltdl

echo "Running ${LIBTOOLIZE}..."
$LIBTOOLIZE --automake --copy --ltdl

if test -d libltdl; then
        echo "Moving libltdl to src .."
        mv libltdl ./src/
fi

if test ! -d src/libltdl ; then
    echo "libltdl for elements exist.  Aborting."
    exit 1
fi

aclocal -I config
# autoheader
autoconf
automake --foreign --add-missing --include-deps
autoreconf --force --install
