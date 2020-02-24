#!/usr/bin/bash
make clean
make

check_output ()
{
	echo "Testing $2"
	if [ "$1" == "success" ]; then
		echo "-Success"
	else
		echo "-Failed, returned $1"
	fi
}

file="testInputs/intTest.txt"
output=$(cat testInputs/intTest.txt | ./parser nodebug)
check_output $output $file

file="testInputs/stringTest.txt"
output=$(cat testInputs/stringTest.txt | ./parser nodebug)
check_output $output $file

file="testInputs/floatTest.txt"
output=$(cat testInputs/floatTest.txt | ./parser nodebug)
check_output $output $file

file="testInputs/boolTest.txt"
output=$(cat testInputs/boolTest.txt | ./parser nodebug)
check_output $output $file

file="testInputs/varTest.txt"
output=$(cat testInputs/varTest.txt | ./parser nodebug)
check_output $output $file
