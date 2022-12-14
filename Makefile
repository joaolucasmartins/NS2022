.PHONY: clean

# https://stackoverflow.com/questions/24736146/how-to-use-virtualenv-in-makefile
venv: venv/touchfile

venv/touchfile: requirements.txt
	test -d venv || python3 -m venv venv
	. venv/bin/activate; pip install -Ur requirements.txt
	@touch venv/touchfile

test: venv
	@chmod +x venv/bin/activate
	. venv/bin/activate && python test.py

clean:
	rm -rf venv
