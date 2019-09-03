#! /bin/bash

make


FILES=`ls ../testcases`;

for X in ${FILES} ; do 
	echo "----RESULTS FOR TESTCASE \"${X}\"----"
	valgrind --leak-check=full --show-leak-kinds=all mymake2 -f ../testcases/${X}

	echo
done