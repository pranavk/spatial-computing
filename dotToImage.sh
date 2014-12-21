# Helper script to write dot format file in form of image.
# First argument should be the name of the dot file.

dot -Tpng $1 > test.png
