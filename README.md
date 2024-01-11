# pico-neuron

Spiking neuron for distributed networks of Raspberry Pi Pico W's 

## Getting Started

* TODO: guide on installing Pico SDK & setup

1. Set the environment variable, `PICO_SDK_PATH`, to the path of your local copy of the [Raspberry Pi Pico C SDK](https://www.raspberrypi.com/documentation/pico-sdk/index_doxygen.html) on your machine:

```shell
export PICO_SDK_PATH=<path to Pi Pico C SDK>
```
2. Run [CMake](https://cmake.org/getting-started/) to generate complete codebase:

```shell
cd <project folder>
cmake . -B build
```

> PRO-TIP: when tinkering, the quickest way to clean & rebuild is `cmake ..` from the build directory

3. Run `GNU Make`

```shell
cd build
make
```

> PRO-TIP: you can put more CPU cores to work with `make -j $(nproc)`. I've been told that only a little hyper-threading (where you can pretend like you have more cores than you do) is often best, so `make -j $((nproc / 2) + 1)`. So I run `make -j 7` on my 6-core machine. Your mileage may vary.

> Don't forget `make clean` between failed attempts.

4.  ...
