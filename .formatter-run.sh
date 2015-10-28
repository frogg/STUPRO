#!/bin/bash

FORMATTER_DIRS=( "src" "test" "include" )

if [[ $1 == "" ]]; then
	COMMAND=astyle
else
	COMMAND=$1
fi


ERROR=0

for i in "${FORMATTER_DIRS[@]}"
do
  $COMMAND --options=.astyle-options-v2 --recursive --suffix=none "$i/*.cpp" "$i/*.hpp" "$i/*.?sh"
done
