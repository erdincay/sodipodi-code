#!/bin/sh

base=$(basename $1 ".svg")

sodipodi ${base}.svg \
	--export-png=${base}.png \
	--export-width=128 \
	--export-background=white
