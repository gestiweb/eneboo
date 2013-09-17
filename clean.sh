make -s uninstall
make -s distclean
find . -name "*~" -exec rm \{\} \;
rm -fR build/lib
rm -fR build/bin/abanq
rm -fR build/include
rm -fR build/mocs
rm -fR build/objs
rm -fR build/rccs
rm -fR build/uics
rm -fR src/gui/images/iconscache/*
cp src/gui/images/nullicon.png src/gui/images/iconscache/
rm gen_settings.pri
  