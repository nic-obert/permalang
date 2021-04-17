#!/bin/bash
find . -name '*.cpp' -o -name '*.hh' | xargs wc -l | sort -nr