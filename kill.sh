#!/bin/bash
ps -ef | grep ./bin/RPi | grep -v grep | awk '{print $2}' | xargs kill
ps -ef | grep cv/main.py  | grep -v grep | awk '{print $2}' | xargs kill
