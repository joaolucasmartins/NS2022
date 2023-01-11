.PHONY: clean cleanall all omnet


INET := /home/tiago/Documents/tum/ns/inet4.4

CONFIGURATIONS := Default NonFrequentUpdates Scalability


all: mobility_configs


# ---------- COMPILING ----------
.PHONY: inet simulations
inet:
	cd ${INET} && make

proj/tum: inet
	cd proj/ && make

simulations: proj/simulations/Default proj/simulations/NonFrequentUpdates proj/simulations/Scalability

proj/simulations/Default: proj/tum mobility_configs
	cd proj/simulations && ../tum\
		-n .:../src:${INET}/src \
		-l ${INET}/src/INET \
		-u Cmdenv \
		-c Default \
		-r 0 \
		-s \
		-f omnetpp.ini

proj/simulations/NonFrequentUpdates: proj/tum mobility_configs
	cd proj/simulations && ../tum\
		-n .:../src:${INET}/src \
		-l ${INET}/src/INET \
		-u Cmdenv \
		-c NonFrequentUpdates \
		-r 0 \
		-s \
		-f omnetpp.ini

proj/simulations/Scalability: proj/tum mobility_configs
	cd proj/simulations && ../tum\
		-n .:../src:${INET}/src \
		-l ${INET}/src/INET \
		-u Cmdenv \
		-c Scalability \
		-r 0 \
		-s \
		-f omnetpp.ini


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
	. venv/bin/activate && cd preprocessing && python export_trains.py offPeak mon 15:00:00 mon 15:05:00 -r S2 -r S8

proj/simulations/mobility/rushHour.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv preprocessing/export_trains.py venv
	. venv/bin/activate && cd preprocessing && python export_trains.py rushHour mon 06:30:00 mon 06:35:00 -r S2 -r S8

proj/simulations/mobility/weekend.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv preprocessing/export_trains.py venv
	. venv/bin/activate && cd preprocessing && python export_trains.py weekend sat 10:00:00 sat 10:05:00 -r S2 -r S8


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