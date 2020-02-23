#!/usr/bin/bash
make
cat $1 | ./parser
bash generate_tree.sh
