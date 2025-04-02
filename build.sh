#!/bin/bash

# Definir variables
TARGET="count"
SRC_DIR="src"
BUILD_DIR="build"

# Encontrar archivos fuente y generar la lista de objetos
SOURCES=$(find $SRC_DIR -name "*.c")
OBJECTS=$(echo "$SOURCES" | sed "s|$SRC_DIR/\(.*\).c|$BUILD_DIR/\1.o|")

# Definir el compilador y las banderas de compilación
CC="gcc"
CFLAGS="-Wall -Wextra -std=c99 -O2"
LIBS="-lm"

mkdir -p $BUILD_DIR

compile() {
  for src in $SOURCES; do
    obj="$BUILD_DIR/$(basename $src .c).o"
    $CC $CFLAGS -c $src -o $obj
  done
}

compile

$CC $CFLAGS $OBJECTS -o $TARGET $LIBS

clean() {
  rm -rf $BUILD_DIR $TARGET
}

if [ "$1" == "clean" ]; then
  clean
fi
