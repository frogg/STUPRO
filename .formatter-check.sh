#!/bin/bash

if [[ $1 == "" ]]; then
	COMMAND=astyle
else
	COMMAND=$1
fi

output=$($COMMAND --options=.astyle-options-v2 --recursive --dry-run "*.cpp" "*.hpp" "*.?sh" 2>&1 | egrep 'Formatted|command not found')
if [[ $output ]]; then
	echo "Formatting error:"
	echo "$output"
	exit 1
else
	exit 0
fi
