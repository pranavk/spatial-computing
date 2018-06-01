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
#include "llvm/IR/Dominators.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/FileSystem.h"
//#include "llvm/Support/GraphWriter.h"

#include "DFGWriter.h"
#include "waves.hpp"
#include "dataflow.hpp"

using namespace llvm;

void printInstructionsWithWaveNo(Function &F, WaveScalar &obj){
  for (inst_iterator I = inst_begin(F), E = inst_end(F); I!=E; ++I){
    Instruction *instr = &*I;
    std::string str;
    raw_string_ostream rso(str);
    instr->print(rso);

    outs() << *instr << " -> " << obj.getWaveNo(str) << "\n";
  }
}

void printDefs(Instruction *instr){
  outs() << "Use :" << *instr << "\n";
  for (User::op_iterator it = instr->op_begin(), e = instr->op_end(); it!=e; it++){
    Instruction *vi = dyn_cast<Instruction>(*it);
    outs() << "\t\t" << *vi << "\n";
  }
}

void printUses(Instruction *instr){
  outs() << "Def :" << *instr << "\n";
  for (Value::use_iterator i = instr->use_begin(), ie = instr->use_end(); i != ie; i++){
    Instruction *vi = dyn_cast<Instruction>(*i);
    outs() << "\t\t" <<*vi << "\n";
  }
}


/*
  DataFlowGraph is a pass that would output the Data flow graph for
  corresponding function in DOT format.
*/
struct DataFlowGraph : public FunctionPass,
                       public SmallVectorImpl <std::pair<const BasicBlock*, const BasicBlock*> > {
  static char ID;
  DataFlowGraph() : FunctionPass(ID), SmallVectorImpl(10){}

  virtual void getAnalysisUsage (AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<DominatorTreeWrapperPass>();
  }

  bool runOnFunction(Function &F) {
    if (F.getName() == "main")
      return false;

    DominatorTreeWrapperPass& DT = getAnalysis<DominatorTreeWrapperPass>();

    SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res = this;
    WaveScalar obj;
    obj.annotateWaves(F, res);

    //    printInstructionsWithWaveNo(F, obj);

    std::vector<Instruction*> worklist;
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I!=E; ++I){
      Instruction *instr = &*I;
      worklist.push_back(instr);
    }

    for (std::vector<Instruction*>::iterator iter = worklist.begin();
         iter != worklist.end();
         iter++){
      Instruction *instr = *iter;
      printUses(instr);
    }

    std::error_code ErrorInfo;
    raw_fd_ostream File("dfg.dot", ErrorInfo, sys::fs::OpenFlags::F_None);
    WriteDFG (File, (DFG<Function*>)&F, obj, DT.getDomTree());

    raw_fd_ostream File1("cfg.dot", ErrorInfo, sys::fs::OpenFlags::F_None);
    WriteGraph (File1, (const Function*)&F);

    return false;
  }
};

char DataFlowGraph::ID = 10;
static RegisterPass<DataFlowGraph> X("dot-dfg",
                                     "Output Dataflow Graph in DOT format for WaveScalar Architecture",
                                     false, false);
