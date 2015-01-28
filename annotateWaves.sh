# Helper Script to run wave annotation.
# First argument should be the file in bitcode format (.bc)

gmake && opt -load ../../../Release+Asserts/lib/Spatial.so -annotateWaves < $1
