#!/usr/bin/env csh
# joosc: compiles JOOS source programs into class files using the A- JOOS
# compiler.
# usage: joosc [-O] f1.java f2.java ... fn.joos
# note: you should name each source file for ordinary classes with
# .java extensions and all external classes with .joos extensions
if ( { $PEEPDIR/joos $* } ) then
	foreach f ( $* )
		if ( $f != "-O" && $f:e != "joos" ) then
			set i = `dirname $f`
			echo $i
			java -jar $PEEPDIR/jasmin.jar $PEEPDIR/$f:r.j -d $i
			if ( $1 == "-O" ) then
				$PEEPDIR/djas -w $f:r.class > ./$f:r.optdump
			else
				$PEEPDIR/djas -w $f:r.class > ./$f:r.dump
			endif
		endif
	end
	echo "----- Method code sizes ----"
	if ( $1 == "-O" ) then
		grep code_length $i/*.optdump
	else
		grep code_length $i/*.dump 
	endif
endif
