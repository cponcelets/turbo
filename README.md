# TURBO _ Python

`Turbo_python` is a build making turbo available in python.
It uses [pybinb11](https://github.com/pybind/pybind11) and follows [cmake example](https://github.com/pybind/cmake_example).

### Dependencies

[Turbo](https://github.com/ptal/turbo)'s dependencies:
* Cuda compiler `nvcc` (>= 12.0).
* [libxml2](http://xmlsoft.org/)
* CMake (>= 3.27)
* Doxygen

Python:
* Python > 10.0
* venv

### Configure, compile and run

Set a virtual environment (venv):
```bash
python3 -m venv turbo-venv
source turbo-venv/bin/activate
pip install --upgrade pip setuptools wheel
pip install pybind11
```

Run pip install wih the preset of your choice:
```
CMAKE_PRESET=<my_preset> pip install .
```

Then copy the library into your pip wheels, for example with the preset `cpu-release-local` and `python3.13`:
```
cp build/cpu-release-local/turbo_python.cpython-313-x86_64-linux-gnu.so turbo-venv/lib64/python3.13/site-packages/
```

### Test

Try if everything went well
```bash
python tests/test.py
```

> Note:
> - preset CPU: We are aware of a bug for big model (turbo runs forever)
>       If you want to stop it open a new terminal, get turbo's PID (commad `top`) and use `kill -3 <turbo's PID>`
> - preset GPU: For WSL you ll need a patch to allow CUDA working properly (see https://github.com/cponcelets/turbo_llm) 
