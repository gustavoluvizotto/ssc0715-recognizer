#!/bin/bash

g++ main.cpp -o a.out `pkg-config --cflags --libs opencv`

