all:
	perl make-java-classes.pl

clean:
	rm -rf tmp

compile:
	(cd tmp && javac `find . -name '*.java'`)
