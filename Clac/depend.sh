#!/bin/bash

# This shell script runs the depend tool to produce output suitable for the Makefile.

depend '-I../Spica;../Scr;clac_entity;clac_engine' depend.dep depend.out
