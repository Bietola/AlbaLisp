#!/bin/bash
cc -std=c99 -g -Wall lispy.c mpc.c -ledit -lm -o lispy
