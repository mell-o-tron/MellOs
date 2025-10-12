 # Contributing to MellOS

Welcome to the MellOS contribution guide!

---

## Finding what to contribute on
A good starting point is to check [issues tagged with "good first issue"](https://github.com/mell-o-tron/MellOS/issues?q=is%3Aissue%20state%3Aopen%20label%3A%22good%20first%20issue%22). These should be simple tasks that we left behind specifically for people who want to contribute but don't know where to start.

If you have issues with those issues, or want to contribute in a different way, you can either open a new issue or reach out to us on [Discord](https://discord.com/invite/DgDDV6xPQe).

---

## Pull Request Process
We don't have a strict pull request process, but here are some guidelines to follow:
1. Fork the repository and create your branch from `main`.
2. :warning: Ensure that your code with all the available options (machine targets, VESA vs TEXT mode, compiler optimizations)
3. :warning: Ensure that your code passes all tests, see details below.
4. Issue that pull request!

:warning: **If you open a pull request to change a big part of the operating system, please get in touch with us first, either by opening an issue or reaching out on Discord. This will help us ensure that your changes align with the project's goals and avoid unnecessary work.** :warning:

---

## Tests

We have a series of tests that you can execute by running MellOS in debug mode, as follows.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TARGET=Debug ..
make debug
```