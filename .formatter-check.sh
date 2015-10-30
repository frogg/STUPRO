#!/bin/bash

FORMATTER_DIRS=( "src" "test" "include" )

if [[ $1 == "" ]]; then
	COMMAND=astyle
else
	COMMAND=$1
fi


ERROR=0

echo "Running $($COMMAND --version)"

for i in "${FORMATTER_DIRS[@]}"
do
  output=$($COMMAND --options=.astyle-options-v2 --recursive --dry-run "$i/*.cpp" "$i/*.hpp" "$i/*.?sh" 2>&1 | egrep 'Formatted|command not found')
  if [[ $output ]]; then
  	echo "Formatting error in $i:"
  	ERROR=1
  fi
done

if [ $ERROR == 0 ]; then
	echo "Formatting seems fine"
fi

exit $ERROR
