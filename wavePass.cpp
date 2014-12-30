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
#include "llvm/Support/GraphWriter.h"

#include "waves.hpp"

using namespace llvm;

/*
  Wave class for waves pass. The main task of this pass is to annotate waves in
  a control flow graph.
*/
struct Waves : public FunctionPass,
               public SmallVectorImpl <std::pair<const BasicBlock*, const BasicBlock*> > {
  static char ID;
  Waves() : FunctionPass(ID), SmallVectorImpl(10){}

  bool runOnFunction(Function &F) {
    if (F.getName() == "main")
      return false;

    SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res = this;
    WaveScalar obj;
    obj.annotateWaves(F, res);
    
    return false;
  }
};

char Waves::ID = 0;
static RegisterPass<Waves> X("annotateWaves",
                             "Annotate Waves in Control Flow Graph",
                             false, false);
