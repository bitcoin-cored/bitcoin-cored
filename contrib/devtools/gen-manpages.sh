#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

CLASHICD=${CLASHICD:-$SRCDIR/clashicd}
CLASHICCLI=${CLASHICCLI:-$SRCDIR/clashic-cli}
CLASHICTX=${CLASHICTX:-$SRCDIR/clashic-tx}
CLASHICQT=${CLASHICQT:-$SRCDIR/qt/clashic_qt}

[ ! -x $CLASHICD ] && echo "$CLASHICD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
BTCVER=($($CLASHICCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for clashicd if --version-string is not set,
# but has different outcomes for clashic_qt and clashic-cli.
echo "[COPYRIGHT]" > footer.h2m
$CLASHICD --version | sed -n '1!p' >> footer.h2m

for cmd in $CLASHICD $CLASHICCLI $CLASHICTX $CLASHICQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${BTCVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${BTCVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
