/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2014 Pranav Kant
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 */

#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>

using namespace llvm;

struct DataFlow : public FunctionPass {
  static char ID;
  DataFlow() : FunctionPass(ID) {}

  bool runOnFunction(Function &F){
    std::vector<Instruction*> worklist;
    for (inst_iterator I = inst_begin(F), E = inst_end(F); I!=E; ++I){
      worklist.push_back(&*I);
      outs() << *I << "\n";
    }
    outs() << "\n\n";

    for (std::vector<Instruction*>::iterator iter = worklist.begin(); iter != worklist.end(); iter++){
      Instruction *instr = *iter;
      outs() << "def : " << *instr << "\n";
      for (Value::use_iterator i = instr->use_begin(), ie = instr->use_end(); i != ie; i++){
        Value *v = *i;
        Instruction *vi = dyn_cast<Instruction>(v);
        outs() << "\t\t" << *vi << "\n";
      }
    }
    
    
    return false;
  }
};

char DataFlow::ID = 0;
static RegisterPass<DataFlow> X("dataflow", "Data Flow Analysis", false, false);
