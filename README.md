<h2 align="center">IOS projekt 2</h2>
<p align="center">
  FIT VUT Brno
  <br>
  <!-- <strong align="center">Use this template for your own projects 😊</strong> -->
</p>

### Requirements 🧾

- `gcc` compiler or another compiler for C
- `make` library to run Makefile
- `Linux / wsl` must be run on linux

### How to run 🏃‍♀️

- clone repozitory `git clone https://github.com/FrantisekSpunda/ios-projekt-2.git`
- got to project folder `cd ios-project-2`
- run `make all` (or `make build` --> `make run`)

### Arguments 📂

Change aruments in `config.cfg` in variable `ARGUMENTS`

```c
...
ARGUMENTS=<L> <Z> <K> <TL> <TB>
...
```

- L: number of skeirs
- Z: number of stops
- K: skibus capacity
- TL: max time, that skier will have breakfast (in ms)
- TB: max time between two stops (in ms)

### Description 📋

Project in winter semester of the first year on FIT VUT in Brno.

Programed in C. Three types of processes: (0) main process, (1) skibus, (2) skiers. Skiers will arrive in random time to stop. Skibus is riding in random time from stop to stop until all skiers are in finish.

There is used shared memory between processes, semaphor for access shared memory and forking processes

### Makefile usage 🐔

Using makefile you can run commands for compilation, generating headers and new modules etc.

| Command             | Description                                                                                                                                                                   |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `help` / `list`     | Show all available commands you can use and their description.                                                                                                                |
| `init`              | Create project configuratin file.                                                                                                                                             |
| `build` / `compile` | Compile C program in src.                                                                                                                                                     |
| `run`               | Run your compiled C program.                                                                                                                                                  |
| `all`               | Compile and run your C program.                                                                                                                                               |
| `clear` / `clean`   | Remove build folder.                                                                                                                                                          |
| `header`            | Update or create header in all `.c` files. In header is your name, copyright and last update of file.                                                                         |
| `module`            | Create new module for C. With this command you will create C module with custom name. In files there will be generated required content that is usualy in all C libraries 👌. |
