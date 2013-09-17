include (./settings.pri)
TEMPLATE = subdirs
SUBDIRS = src
MAKE = make \
    -s
MAKE_FILE = Makefile.Release
copyheaders.commands = $$MAKE \
    -C \
    $$ABANQSRCDIR/src/sql \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/corelib \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/acl \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/$$AQUISRC_DIR \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/network \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/script \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/xml \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/3rdparty/sha1 \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/3rdparty/lrelease \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/3rdparty/barcode \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/3rdparty/kugar \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/3rdparty/pq \
    -f \
    Makefile \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE \
    -C \
    $$ABANQSRCDIR/src/3rdparty/qsa/src/qsa \
    -f \
    $$MAKE_FILE \
    compiler_install_headers_make_all \
    ;
copyheaders.commands += $$MAKE
copyheaders.target = copyheaders
QMAKE_EXTRA_TARGETS += copyheaders

