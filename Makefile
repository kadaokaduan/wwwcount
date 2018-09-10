#############################################################
# Master Makefile for the WWW Homepage Access Counter
# Muhammad A Muquit
# Nov-08-1997, created for Count 2.4
#############################################################
first:
	@echo "=========================================================="
	@echo "Please type the approptiate tag with make"
	@echo " all       - compile all with default settings in src/config.h"
	@echo ""
	@echo " makefiles - generate Makefiles"
	@echo " all_libs  - make the libraries"
	@echo " Count.cgi - compile the Count.cgi program"
	@echo " mkstrip   - compile the mkstrip utility"
	@echo " extdgts   - compile the extdgts utility"
	@echo " config.h  - generate the config.h header (interactive)"
	@echo " cfg       - generate count.cfg file"
	@echo " install   - install everything (interactive)"
	@echo " clean     - remove object all object files etc"
	@echo " pristine  - clean real good"
	@echo "=========================================================="

all: makefiles all_libs Count.cgi mkstrip extdgts lbin

makefiles:
	@echo "================ making Makefiles"
	./configure

all_libs:
	@echo "================= making libmcfg.a"
	(cd libs/mcfg; make)
	@echo "================= making libCombine.a"
	(cd libs/combine; make)
	@echo "================= making libbigplus.a"
	(cd libs/bigplus; make)

Count.cgi:
	@echo "================= making Count.cgi"
	(cd src; make)

mkstrip:
	@echo "================== making mkstrip"
	(cd utils/mkstrip; make)

extdgts:
	@echo "================== making extdgts"
	(cd utils/extdgts; make)

lbin:
	/bin/ls -l ./bin

install:
	(sh ./Count-install)

config.h:
	(sh ./Count-config)

cfg:
	(sh ./Gen-conf)

clean:
	(cd libs/mcfg; make clean)
	(cd libs/combine; make clean)
	(cd libs/bigplus; make clean)
	(cd src; make clean)
	(cd utils/mkstrip; make clean)
	(cd utils/extdgts; make clean)

pristine:
	rm -f bin/Count.cgi bin/mkstrip bin/extdgts
	(cd libs/mcfg; make clean)
	(cd libs/combine; make clean)
	(cd libs/bigplus; make clean)
	(cd src; make clean)
	(cd utils/mkstrip; make clean)
	(cd utils/extdgts; make clean)
	rm -f libs/combine/Makefile
	rm -f libs/bigplus/Makefile
	rm -f utils/mkstrip/Makefile
	rm -f config.cache config.log config.status
