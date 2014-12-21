spatial-computing
=================

The repository is manifestation of dataflow computer architecture or computation
in space.

We are using LLVM compiler infrastructure to manifest something similar to
Wavescalar computer architecture. Taking ideas from Wavescalar computer
architecture, we would be using the concept of waves and create a framework that
would output the systemC implementation automatically from any given procedure
in imperative languages.

Using LLVM format is quite helpful here since it provides front end for many
languages and convert all of them into a Common Intermediate representation
(IR). This way we don't have to deal separately with so many different
languages. Instead we just play with IR and proceed.

Hence automatic hardware synthesis for procedures given in imperative languages
can be used immensely to enhance the performance of ASICs using this dataflow
computer architecture.
