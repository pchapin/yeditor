#!/bin/bash

# This shell script runs the depend tool to produce output suitable for the Makefile.

depend '-I../../../Spica/Cpp;../clac_entity' depend.dep depend.out
