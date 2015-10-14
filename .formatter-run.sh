#!/bin/bash

if [[ $1 == "" ]]; then
	COMMAND=astyle
else
	COMMAND=$1
fi

$COMMAND --options=.astyle-options-v2 --recursive --suffix=none "*.cpp" "*.hpp" "*.?sh"
