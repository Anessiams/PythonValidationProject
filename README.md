# PythonValidationEngine

A tool to validate files using user-defined policy files implemented using python scripts. The engine is split into two components - a handler and a validator. The handler supervises the validator process(es) and communicates using Linux IPC mechanisms such as shared memory and message queues. The handler is responsible for orchestrating IPC and synchronization mechanisms while the validator is responsible for consuming IPC mechanisms and invoking the user provided policy file scripts.

## Usage

### Build Handler
You can build the C++ handler using cmake or gcc.

```console
user@machine:~/ValidationEngine/handler$ cmake --build . --target ValidationEngineExe
```
OR
```console
user@machine:~/ValidationEngine/handler$ g++ src/!(*.test).cpp src/!(*.test).h -o ValidationEngineExe
```

### Modify Configuration File
A default configuration file is contained inside `~/ValidationEngine/handler/config.yml`. 
The file uses the yaml format. You can modify the validation container path, add more policy file scripts,
or configure IPC mechanisms. 

```yaml
---
# Configuration yaml file
validator:
  container: ../test-policy
  policies:
    policy: ../test-policy.py
    policy-1: ../test-policy-1.py
    policy-2: ../test-policy-2.py
  instances: 1
handler:
  log-levels: 
    info: 1
    error: 2
```

### Writing Policy Files
TODO

### Use the Engine
You can execute the handler using `./ValidationEngineExe config.exe`. 
The handler accepts a single argument, the path to the configuration file to use.
Since the configuration file paths are opened by the handler program, make sure the paths in the config
are relative to the handler executable and not the configuration file itself.
Alternatively, use absolute file paths.

```console
user@machine:~/ValidationEngine/handler$ ./ValidationEngineExe ../path/to/config.yaml
```

Enter a file to validate into stdin to validate it. Output for the file will be on stdout when validation is complete.
Make sure the input file path is relative to the handler or is absolute.

```console
user@machine:~/ValidationEngine/handler$ ./ValidationEngineExe ../path/to/config.yaml
../path/to/file.txt
../path/to/file.txt 1
```
