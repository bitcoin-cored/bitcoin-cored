#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

CORED=${CORED:-$SRCDIR/cored}
CORECLI=${CORECLI:-$SRCDIR/core-cli}
CORETX=${CORETX:-$SRCDIR/core-tx}
COREQT=${COREQT:-$SRCDIR/qt/core_qt}

[ ! -x $CORED ] && echo "$CORED not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
BTCVER=($($CORECLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for cored if --version-string is not set,
# but has different outcomes for core_qt and core-cli.
echo "[COPYRIGHT]" > footer.h2m
$CORED --version | sed -n '1!p' >> footer.h2m

for cmd in $CORED $CORECLI $CORETX $COREQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${BTCVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${BTCVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
