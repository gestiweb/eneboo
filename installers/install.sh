#!/bin/bash
APP_NAME="Eneboo 2.4.0.1 dbAdmin Linux 32bits"
INSTALL_PARENT="/opt"
INSTALL_FOLDER="eneboo-2.4"
INSTALL_PATH="$INSTALL_PARENT/$INSTALL_FOLDER"
SYMLINK_PATH="/usr/local/bin"
APP_DESKTOP="Eneboo.desktop"
test $(whoami) = "root" || {
    echo "Este programa necesita permisos de root para instalar. Intentando 'sudo' . . ."
    exec sudo "$0"
}

echo "Instalando $APP_NAME en $INSTALL_PATH . . ."

test -e "$INSTALL_PATH" && {
    echo "El directorio ya existía, se borra."
    rm -Rf "$INSTALL_PATH"
}
mkdir "$INSTALL_PATH"

cp * "$INSTALL_PATH" -R

for user in $(grep /home /etc/passwd | grep -Eo "^[^:]+"); do
    desktop=$(su --command "xdg-user-dir DESKTOP" "$user" 2>/dev/null)
    if [ -d $desktop ] ; then
        DST="$desktop/$APP_DESKTOP"  
        cp "$APP_DESKTOP" "$DST"  
        chown $user "$DST"  
    fi
done



for f in "$INSTALL_PATH"/bin/*; do
    fbase=$(basename "$f")
    unlink "$SYMLINK_PATH/$fbase" 2>/dev/null
    ln -s "$f" "$SYMLINK_PATH/$fbase"
done

echo "$APP_NAME ha sido instalado correctamente."





