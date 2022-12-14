.PHONY: clean

# https://stackoverflow.com/questions/24736146/how-to-use-virtualenv-in-makefile
venv: venv/touchfile

venv/touchfile: requirements.txt
    test -d venv || virtualenv venv
    . venv/bin/activate; pip install -Ur requirements.txt
    touch venv/touchfile

test: venv
    . venv/bin/activate; test

clean:
    rm -rf venv
    find -iname "*.pyc" -delete # What does this line do?

