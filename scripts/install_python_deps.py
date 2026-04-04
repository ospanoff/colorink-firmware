# Installs packages from requirements.txt into PlatformIO Core's Python venv.
# https://docs.platformio.org/en/latest/scripting/examples/extra_python_packages.html

Import("env")

from pathlib import Path

req = Path(env["PROJECT_DIR"]) / "requirements.txt"
if req.is_file():
    env.Execute(f'$PYTHONEXE -m pip install -q -r "{req}"')
