spatial-computing
=================

This repository contains the LLVM compiler passes that are used to convert a program written in C/C++ to a synthesizable dataflow intermediate representation, called Program Dependence Mesh (PDM). PDM is our own intermediate representation based on Program Dependence Web (PDW), Program Dependence Graph (PDG), etc. It is a synthesizable representation unlike others. 

The target of the PDM is a unified platform that can be used as an ASIC, a CPU, or a CGRA.

We use the concept of waves, taken from WaveScalar Architecture, to solve synchronization problems, especially in nested loops and recursion. This compiler pass outputs the PDM in .dot format which can be easily converted to an image and parsed by SystemC for architectural exploration and analysis of the project.

Using LLVM format is quite helpful here since it provides front end for many languages and convert all of them into a Common Intermediate representation (IR). Having said that, the pass should be able to get PDM from any language which has a LLVM frontend; though, this is not something I have tried.

You can read [this](http://pranavk.github.io/architecture/spatial-computing/) blog post giving an overview of the project.
