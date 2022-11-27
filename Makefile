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
INPUT     = ${INPUTDIR}/input.txt

all:
	clear
	@echo "****************************"
	@echo "*                          *"
	@echo "* compiling pgrep		  *"
	@echo "*                          *"
	@echo "****************************"
	gcc -pthread ${GREP} -o pgrep

regex:
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
	./a.out ${INPUT}


	
