#!/bin/bash
#
# This script cleans clac by first running 'make clean' in the two subprojects and then in the
# main folder as well.

(cd clac_entity; make clean)
(cd clac_engine; make clean)
make clean
