/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2014 Pranav Kant
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 */

#include "llvm/ADT/DenseMap.h"
#include "llvm/Analysis/CFGPrinter.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SCCIterator.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/DataFlow.h"
#include "llvm/Support/GraphWriter.h"

#include <vector>
#include "waves.hpp"
#include "dataflow.hpp"

using namespace llvm;
/*
DFGWriter::DFGWriter(raw_ostream &o, const DFG<Function*> &F, bool SN)
  : GraphWriter<DFG<Function*> > (o, F, SN), O(o) { }

void DFGWriter::writeHeader(const std::string &Title){
  O << "digraph pranav { \n";
}

void DFGWriter::writeGraph(const std::string &Title){
  outs() << "things\n";
  writeNodes();
  writeFooter();
}

raw_ostream &WriteDFG(raw_ostream &O,
                      const DFG<Function*> &G,
                      bool shortNames,
                      const Twine &Title) {
  GraphWriter<DFG<Function*> > W(O, G, shortNames);
  W.writeGraph(Title.str());
  return O;
}



*/
