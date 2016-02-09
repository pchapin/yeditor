#
# FILE   : Makefile
# SUBJECT: Makefile for the Spica/Cpp documentation.
# AUTHOR : Peter C. Chapin
#

# Implicit Rules
################
%.pdf:	%.svg
	inkscape -z --file=$< --export-pdf=$@

all:	BoundedList.pdf

# Figures
BoundedList-Representation.pdf:	BoundedList-Representation.svg

# Compile the documentation.
BoundedList.pdf:	BoundedList.tex BoundedList-Representation.pdf
	pdflatex BoundedList.tex

clean:
	rm -f *.aux *.log *.pdf
