.PHONY: clean cleanall all omnet

include config.mk

CONFIGURATIONS := Default NonFrequentUpdates Scalability

OMNET_FLAGS = -n .:../src:${INET}/src:${SIMU5G}/src \
		-l ${INET}/src/INET \
		-l ${SIMU5G}/src/simu5g \
		-u Cmdenv \
		-r 0 \
		-s \
		-f omnetpp.ini

all: mobility_configs


# ---------- COMPILING ----------
.PHONY: libs simulations
${INET}/src/libINET.so:
	cd ${INET} && make

${SIMU5G}/src/libsimu5g.so:
	cd ${SIMU5G} && make

libs: ${INET}/src/libINET.so ${SIMU5G}/src/libsimu5g.so

proj/tum: libs
	cd proj/ && opp_makemake -f --deep -O out -o tum -KINET_PROJ=${INET} -KSIMU5G_PROJ=${SIMU5G} \
		-I. -I$$\(INET_PROJ\)/src -I$$\(SIMU5G_PROJ\)/src \
		-L$$\(INET_PROJ\)/src -L$$\(SIMU5G_PROJ\)/src \
		-lINET$$\(D\)\
		-DINET_IMPORT
	cd proj/ && make

simulations: proj/simulations/Default proj/simulations/NonFrequentUpdates proj/simulations/Scalability

proj/simulations/Default: proj/tum mobility_configs
	cd proj/simulations && ../tum -c Default ${OMNET_FLAGS}

proj/simulations/NonFrequentUpdates: proj/tum mobility_configs
	cd proj/simulations && ../tum -c NonFrequentUpdates ${OMNET_FLAGS}

proj/simulations/Scalability: proj/tum mobility_configs
	cd proj/simulations && ../tum -c Scalability ${OMNET_FLAGS}


# ---------- RESULT COLLECTINO & PLOTTING ----------
.PHONY: plots
plots:
	. venv/bin/activate && cd result_pipeline && python parseData.py ../proj/simulations/results/ Default-#0
	. venv/bin/activate && cd result_pipeline && python parseData.py ../proj/simulations/results/ NonFrequentUpdates-#0
	. venv/bin/activate && cd result_pipeline && python parseData.py ../proj/simulations/results/ Scalability-#0




# ---------- PREPROCESSING ----------
.PHONY: mobility_configs

preprocessing/muenchen/denormalized.csv: preprocessing/original preprocessing/preprocessing.py venv
	. venv/bin/activate && cd preprocessing && python preprocessing.py

proj/simulations/mobility:
	mkdir proj/simulations/mobility -p

mobility_configs: proj/simulations/mobility/offPeak.ini proj/simulations/mobility/rushHour.ini proj/simulations/mobility/weekend.ini

proj/simulations/mobility/offPeak.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv preprocessing/export_trains.py venv
	. venv/bin/activate && cd preprocessing && python export_trains.py offPeak mon 15:00:00 mon 16:00:00 -r S2 -r S8 --timeScale 10

proj/simulations/mobility/rushHour.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv preprocessing/export_trains.py venv
	. venv/bin/activate && cd preprocessing && python export_trains.py rushHour mon 06:30:00 mon 07:30:00 -r S2 -r S8 --timeScale 10

proj/simulations/mobility/weekend.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv preprocessing/export_trains.py venv
	. venv/bin/activate && cd preprocessing && python export_trains.py weekend sat 10:00:00 sat 11:00:00 -r S2 -r S8 --timeScale 10


# ---------- Python Virtual Env ----------
venv: venv/touchfile

venv/touchfile: requirements.txt
	test -d venv || python3 -m venv venv
	. venv/bin/activate; pip install -Ur requirements.txt
	@touch venv/touchfile

clean:
	rm -rf */__pycache__
	rm -rf proj/simulations/results
	cd proj && make clean

cleanall: clean
	rm -rf venv
	rm -rf preprocessing/muenchen
	cd proj/simulations/mobility && rm -f offPeak.* rushHour.* weekend.*
	cd proj && make cleanall