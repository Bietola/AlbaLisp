#!/bin/bash
cc -std=c99 -g -Wall parsing.c mpc.c -ledit -lm -o parsing
