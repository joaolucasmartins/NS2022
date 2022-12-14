.PHONY: clean cleanall preprocessing mobility_configs all

all: preprocessing

omnet:
	@echo "Nani should I do here?"

preprocessing: mobility_configs

preprocessing/muenchen: preprocessing/original preprocessing/preprocessing.py venv
	. venv/bin/activate && cd preprocessing && python preprocessing.py

mobility_configs: preprocessing/muenchen venv
	mkdir preprocessing/muenchen -p
	. venv/bin/activate && python preprocessing/export_trains.py offPeak mon 10:00:00 mon 12:00:00
	. venv/bin/activate && python preprocessing/export_trains.py rushHour mon 06:00:00 mon 08:00:00
	. venv/bin/activate && python preprocessing/export_trains.py weekend sat 10:00:00 sat 12:00:00

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
