#!/bin/bash
gcc board.c search.c tests.c -o tests
./tests v testcases/victory/*.txt testcases/victory/generated/*.txt
./tests o testcases/opportunities/*.txt
./tests m testcases/move_choice/*.txt
