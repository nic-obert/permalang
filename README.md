# **Permalang**

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

A **toolchain** for the Permalang language including:
- **permalang compiler**
- **permalang virtual machine**

Inspired by the C programming language

<br>

---
<br>

## **Table of contents**

- [**Permalang**](#permalang)
  - [**Table of contents**](#table-of-contents)
  - [**Installation**](#installation)
  - [**Usage**](#usage)
  - [**Syntax and features**](#syntax-and-features)
  - [**Data types**](#data-types)
  - [**License**](#license)

<br>

---
<br>

## **Installation**

Build toolchain
```bash
make release
```
Install toolchain
```bash
make install
```
Uninstall toolchain
```bash
make uninstall
```

<br>

---
<br>

## **Usage**

Compile
```bash
pcc <source.pf>
```
Compile with optimizations
```bash
pcc -O <source.pf>
```
Use the `-v` flag for verbose compilation

<br>

Run executable
```bash
pcc <executable> -x
```
Help page
```bash
pcc --help
```

<br>

---
<br>

## **Syntax and features**

Most of the syntax is borrowed from C

Variable declaration and assignment
```c
int i;
int i = 0;
```

If statements
```c
if (/* condition */)
{
    // code
}
```

While loop
```c
while (/* condition */)
{
    // code
}
```

<br>

---
<br>

## **Data types**

```c
bool b = false;
byte b = 0;
int i = 0;
long l = 0;
float f = 0;  // still work in progress
double d = 0; // still work in progress
```

<br>

---
<br>

## **License**

[**GNU GPLv3**](LICENSE)
