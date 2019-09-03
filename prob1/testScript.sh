#! /bin/bash

make


FILES=`ls ../testcases`;

for X in ${FILES} ; do 
	mymake2 -f ../testcases/${X} &>myOut
	echo $? >>myOut
	echo "----RESULTS FOR TESTCASE \"${X}\"----"
	#cat myOut
	#echo
	../assg10Ex/prob1/ex_mymake2 -f ../testcases/${X} &>exOut
	echo $? >>exOut
	#cat exOut
	diff -Z myOut exOut
	echo
done