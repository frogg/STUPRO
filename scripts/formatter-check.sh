#!/bin/bash

FORMATTER_DIRS=( "src" "test" )

COMMAND=astyle
PRINT_ASTYLE_VERSION=false
BLAME=true
PRINT_DIR_HEADERS=true
ONLY_OWN=false

# check command line arguments
while test $# -gt 0; do
	case "$1" in
		--print-version)
			PRINT_ASTYLE_VERSION=true
			;;
		# disables blame feature
		--no-blame)
			BLAME=false
			;;
		# disable 'Formatting error in [dir]' output
		--no-dir-headers)
			PRINT_DIR_HEADERS=false
			;;
		# only check for errors introduced by the current user
		--only-own)
			ONLY_OWN=true
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

if [ "$PRINT_ASTYLE_VERSION" == true ]; then
	echo "Running $($COMMAND --version)"
fi

# initilize error flag to 0
ERROR=0

if [ "$ONLY_OWN" == true ]; then
	USERNAME=$(git config user.name)
fi

EXEC_BLAME=false
if [ "$BLAME" == true ]; then
	EXEC_BLAME=true
else
	if [ "$ONLY_OWN" == true ]; then
		EXEC_BLAME=true
	fi
fi

# only check directories in the FORMATTER_DIRS list
for i in "${FORMATTER_DIRS[@]}"
do
  output=$($COMMAND --options=.astyle-options --recursive --dry-run "$i/*.cpp" "$i/*.hpp" "$i/*.?sh" 2>&1 | egrep 'Formatted|command not found')
  if [[ $output ]]; then
		DIR_HEADER="\e[41m\e[97mFormatting error in $i\e[49m\e[39m"
		PRINTED_DIR_HEADER=false
		if [ "$ONLY_OWN" == false ]; then
			if [ "$PRINT_DIR_HEADERS" == true ]; then
				# print error message (white font on red background)
		  	echo -e "$DIR_HEADER"
				PRINTED_DIR_HEADER=true
			fi
			# set error flag
	  	ERROR=1
		fi

		# iterate over every line printed by astyle
		while read -r line; do
			# extract the paths of all malformed files
			fileregex="Formatted  (.*)"
			[[ $line =~ $fileregex ]]
			file="$i/${BASH_REMATCH[1]}"

			if [ "$EXEC_BLAME" == true ]; then
				# look up the person responsible for the formatting issue
				blame=$(git log -n 1 --format=format:"last commit by %Cgreen%an%Creset: (%C(auto)%h%Creset) %Cblue%s%Creset" -- $file)
				if [ "$ONLY_OWN" == true ]; then
					if [[ $blame == *"$USERNAME"* ]]; then
						ERROR=1
						if [ "$PRINT_DIR_HEADERS" == true ]; then
							if [ "$PRINTED_DIR_HEADER" == false ]; then
								echo -e "$DIR_HEADER"
								PRINTED_DIR_HEADER=true
							fi
						fi
						if [ "$BLAME" == true ]; then
							echo -e "\e[91m$file\e[39m: $blame"
						else
							echo "$file"
						fi
					fi
				else
					ERROR=1
					echo -e "\e[91m$file\e[39m: $blame"
				fi
			else
				echo "$file"
				ERROR=1
			fi
		done <<< "$output"
  fi
done

if [ $ERROR == 0 ]; then
	echo -e "\e[32mFormatting seems fine\e[39m"
fi

# always return 0 until the repository is in a state that allows proper blaming
# exit 0
exit $ERROR
