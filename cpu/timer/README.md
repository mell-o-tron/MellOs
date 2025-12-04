[← CPU README](../README.md) | [← Main README](../../README.md)

# timer

This directory contains code for timer management in MellOs, including PIT setup and timer interrupt handling.

## timer.c / timer.h
- **timer_phase(int hz)**: Sets the timer frequency (PIT divisor).
- **timer_handler(regs \*r)**: Handles timer interrupts (increments tick counters, triggers events).
- **timer_install()**: Installs the timer interrupt handler.
- **sleep(int ticks)**: Busy-waits for a number of timer ticks.
- **timer_ticks**: Global variable tracking the number of timer interrupts.
- **seconds**: Global variable tracking elapsed seconds.
