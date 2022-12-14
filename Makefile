.PHONY: clean cleanall all omnet


INET = ~/Documents/tum/ns/inet4.4


all: mobility_configs


inet:
	cd ${INET} && make

tum: inet
	cd proj/ && make

simulations: tum mobility_configs
	cd proj/simulations && ../tum\
		-n .:../src:${INET}/src \
		-l ${INET}/src/INET \
		-u Cmdenv \
		-c Default \
		-r 0 \
		-s \
		-f omnetpp.ini



# ---------- PREPROCESSING ----------
.PHONY: mobility_configs

preprocessing/muenchen/denormalized.csv: preprocessing/original preprocessing/preprocessing.py venv
	. venv/bin/activate && cd preprocessing && python preprocessing.py

proj/simulations/mobility:
	mkdir proj/simulations/mobility -p

mobility_configs: proj/simulations/mobility/offPeak.ini proj/simulations/mobility/rushHour.ini proj/simulations/mobility/weekend.ini

proj/simulations/mobility/offPeak.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv preprocessing/export_trains.py venv
	. venv/bin/activate && cd preprocessing && python export_trains.py offPeak mon 14:00:00 mon 16:00:00

proj/simulations/mobility/rushHour.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv preprocessing/export_trains.py venv
	. venv/bin/activate && cd preprocessing && python export_trains.py rushHour mon 06:00:00 mon 08:00:00

proj/simulations/mobility/weekend.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv preprocessing/export_trains.py venv
	. venv/bin/activate && cd preprocessing && python export_trains.py weekend sat 10:00:00 sat 12:00:00


# ---------- Python Virtual Env ----------
venv: venv/touchfile

venv/touchfile: requirements.txt
	test -d venv || python3 -m venv venv
	. venv/bin/activate; pip install -Ur requirements.txt
	@touch venv/touchfile

test: venv
	. venv/bin/activate && python test.py

clean:
	rm -rf *\__pycache__

cleanall: clean
	rm -rf venv
	rm -rf preprocessing/muenchen
	cd proj/simulations/mobility && rm -f offPeak.* rushHour.* weekend.*
