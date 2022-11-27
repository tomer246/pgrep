###############
# DIRECTORIES #
###############
BASEDIR           = $(shell pwd)
INPUTDIR 		  = ${BASEDIR}/input
#########
# FILES #
#########
REGEX 	= ${BASEDIR}/regex.c
GREP 	= ${BASEDIR}/pgrep.c
INPUT2     = ${INPUTDIR}/input.txt
INPUT 		= ${BASEDIR}/input.txt

all:
	clear
	@echo "****************************"
	@echo "*                          *"
	@echo "* compiling				  *"
	@echo "*                          *"
	@echo "****************************"
	gcc ${REGEX} -o a.out
	@echo "****************************"
	@echo "*                          *"
	@echo "* running on input         *"
	@echo "*                          *"
	@echo "****************************"
	./a.out ${INPUT2}

grep:
	clear
	@echo "****************************"
	@echo "*                          *"
	@echo "* compiling pgrep		  *"
	@echo "*                          *"
	@echo "****************************"
	gcc -pthread ${GREP} -o a.out
	./a.out -f ${INPUT} -p abc
	
