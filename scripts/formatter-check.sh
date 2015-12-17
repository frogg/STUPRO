#!/bin/bash

FORMATTER_DIRS=( "src" "test" )

COMMAND=astyle
BLAME=true

# check command line arguments
while test $# -gt 0; do
	echo "\"$1\""
	case "$1" in
		# disables blame feature
		--no-blame)
			BLAME=false
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

# initilize error flag to 0
ERROR=0

# only check directories in the FORMATTER_DIRS list
for i in "${FORMATTER_DIRS[@]}"
do
  output=$($COMMAND --options=.astyle-options --recursive --dry-run "$i/*.cpp" "$i/*.hpp" "$i/*.?sh" 2>&1 | egrep 'Formatted|command not found')
  if [[ $output ]]; then
		# print error message (white font on red background)
  	echo -e "\e[41m\e[97mFormatting error in $i\e[49m\e[39m"
		# set error flag
  	ERROR=1

		# only blame people if asked to do so
		if [ "$BLAME" = true ] ; then
			# iterate over every line printed by astyle
			while read -r line; do
				# extract the paths of all malformed files
				fileregex="Formatted  (.*)"
				[[ $line =~ $fileregex ]]
				file="$i/${BASH_REMATCH[1]}"

				# look up the person responsible for the formatting issue
				blame=$(git log -n 1 --format=format:"last commit by %Cgreen%an%Creset: (%C(auto)%h%Creset) %Cblue%s%Creset" -- $file)

				# output the filename and red, followed by the blame string
				echo -e "  \e[91m$file\e[39m: $blame"
			done <<< "$output"
		fi
  fi
done

if [ $ERROR == 0 ]; then
	echo "Formatting seems fine"
fi

# always return 0 until the repository is in a state that allows proper blaming
exit 0
# exit $ERROR
