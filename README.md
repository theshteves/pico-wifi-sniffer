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

4. Cry over build or linker errors at 99% that are almost definitely your fault. Reconsider many life choices. Find the missing semicolon. Get the missing header into the right folder. Rinse & repeat. Return to Step 1.

> **Don't forget to triple-check that your local Pico SDK hasn't fallen out-of-sync with upstream changes, ESPECIALLY the sneaky submodules hidden under `pico-sdk/lib/` that are maintained by independent groups!** Like any great project (especially new ones), there's consant bugfixing & subtle compatibility issues that are always being tweaked for better & sometimes worse.

5. 🎉 Show Time! Connect your Raspberry Pi Pico W to your computer & "flash" your program onto its tiny chip.

```shell
cp neuron.uf2 <path-to-USB-folder>
```

5. Watch stdout from terminal with [GNU Screen](). This step is the most OS-specific/sensitive:

```shell
screen /dev/<device-name> 115200
```

> For Linux/MacOS , if you're not perfectly comfortable with the complete linux device model like everyone else, you can carefully study the difference between `ls /dev` run _before AND after_ you connect your device. It might be /dev/ttyUSB123 or something similar. Not only will this name be OS-specific, but it will also be very "physical connection"-specific ...by design.

I've tried to eliminate this "magic number" of `115200` from this workflow, but it's critical to commnuicate with the Pico W. This number synchronizes with the Pico's [baud rate](). Aren't you glad I started documenting this? You're welcome.
