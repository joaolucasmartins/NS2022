.PHONY: clean cleanall preprocessing mobility_configs all

all: preprocessing

omnet:
	@echo "Nani should I do here?"


# ---------- PREPROCESSING ----------
preprocessing: preprocessing/muenchen/denormalized.csv mobility_configs

preprocessing/muenchen/denormalized.csv: preprocessing/original preprocessing/preprocessing.py venv
	. venv/bin/activate && cd preprocessing && python preprocessing.py

proj/simulations/mobility:
	mkdir proj/simulations/mobility -p

mobility_configs: proj/simulations/mobility/offPeak.ini proj/simulations/mobility/rushHour.ini proj/simulations/mobility/weekend.ini

proj/simulations/mobility/offPeak.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv venv
	. venv/bin/activate && cd preprocessing && python export_trains.py offPeak mon 14:00:00 mon 16:00:00

proj/simulations/mobility/rushHour.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv venv
	. venv/bin/activate && cd preprocessing && python export_trains.py rushHour mon 06:00:00 mon 08:00:00

proj/simulations/mobility/weekend.ini: proj/simulations/mobility preprocessing/muenchen/denormalized.csv venv
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
