#!/bin/bash

export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH

../../bin/tast_drive libtast-basiccpp.so $@ 
