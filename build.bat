@echo off

set venvs=venv38-64
set venvs=%venvs%;venv38-32
set venvs=%venvs%;venv37-64
set venvs=%venvs%;venv37-32
set venvs=%venvs%;venv36-64
set venvs=%venvs%;venv36-32
set venvs=%venvs%;venv34-64
set venvs=%venvs%;venv34-32
Rem set venvs=%venvs%;venv27-64
Rem set venvs=%venvs%;venv27-32

(for %%v in (%venvs%) do (
    echo Running %%v
    call "%%v\Scripts\activate.bat"

    pip wheel . --no-deps -w dist
    pip install -e .
    cd tests
    python test_pyglobal.py
    python test_cglobal.py
    cd ..
    call deactivate
    echo.
))
