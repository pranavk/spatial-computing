# Helper script to write dot file from given C file.
# First argument : Name of the C file.
# Second argument : 'opt' if you want to optimize the code

clang -c -emit-llvm $1
arg=$1
if [ "$2" == "opt" ]; then
	opt -O1 -o ${arg%.*}.bc ${arg%.*}.bc
fi
opt --dot-cfg-only ${arg%.*}.bc
