#!/bin/bash

FORMATTER_DIRS=( "src" "test" )

COMMAND=astyle
ONLY_OWN=true

# check command line arguments
while test $# -gt 0; do
	case "$1" in
		# format all files and not only those that were most recently changed by the current git user
		--all)
			ONLY_OWN=false
			;;
		# specify an alternate stylecheck command
		--command)
			shift
			if test $# -gt 0; then
				COMMAND=$1
			else
				echo "no alternate command specified"
				exit 1
			fi
			;;
		# reached the end of the argument list
		*)
			break
			;;
	esac
	# continue with the next argument
	shift
done
echo "Running $($COMMAND --version)"

if [ "$ONLY_OWN" == true ]; then
	CHECKER_OUTPUT=$(./scripts/formatter-check.sh --command $COMMAND --no-blame --no-dir-headers --only-own)
	if [ $? == 0 ]; then
		echo "Found no files to format for the current git user"
		exit 0
	else
		while read -r line; do
			$COMMAND --options=.astyle-options --suffix=none "./$line"
		done <<< "$CHECKER_OUTPUT"
	fi
else
	for i in "${FORMATTER_DIRS[@]}"
	do
	  $COMMAND --options=.astyle-options --recursive --suffix=none "$i/*.cpp" "$i/*.hpp" "$i/*.cxx" "$i/*.h" "$i/*.?sh"
	done
fi
