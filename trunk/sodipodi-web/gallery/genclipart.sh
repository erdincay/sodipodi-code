#!/bin/sh

base=$(basename $1 ".svg")

sodipodi ${base}.svg \
	--export-png=${base}.png \
	--export-height=128 \
	--export-background=white
