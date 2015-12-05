#!/bin/bash

FORMATTER_DIRS=( "src" "test" )

if [[ $1 == "" ]]; then
	COMMAND=astyle
else
	COMMAND=$1
fi

for i in "${FORMATTER_DIRS[@]}"
do
  $COMMAND --options=.astyle-options-v2 --recursive --suffix=none "$i/*.cpp" "$i/*.hpp" "$i/*.?sh"
done
