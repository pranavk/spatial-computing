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

#include "waves.h"

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

  bool runOnFunction(Function &F) {
    if (F.getName() == "main")
      return false;

    SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res = this;
    WaveScalar obj;
    obj.annotateWaves(F, res);
    outs() << "Outputting CFG Only ...\n";
    F.viewCFGOnly();
    outs() << "Outputting CFG ...\n";
    F.viewCFG();

    printInstructionsWithWaveNo(F, obj);

    outs() << "strict diGraph G{\n";
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
    /*
    for (std::vector<Instruction*>::iterator iter = worklist.begin(); iter != worklist.end(); iter++){
      Instruction *instr = *iter;
        std::string ppstr;
        raw_string_ostream rso1(ppstr);
        instr->print(rso1);
        std::string pstr = EscapeString(ppstr);


        for (Value::use_iterator i = instr->use_begin(), ie = instr->use_end(); i != ie; i++){
          Value *v = *i;
          Instruction *vi = dyn_cast<Instruction>(v);
          std::string str;
          raw_string_ostream rso(str);
          vi->print(rso);
          std::string istr = EscapeString(str);

          if (!obj.isSameWave(instr, vi))
            outs() << "\"" <<pstr << "\"" << WA() << "\"" <<istr << "\";\n";
          else
            outs() << "\"" << pstr << "\" -> \"" << istr << "\";\n";
        }
    }

    for (std::vector<Instruction*>::iterator iter = worklist.begin(); iter!=worklist.end(); iter++){
      Instruction *instr = *iter;
        std::string ppstr;
        raw_string_ostream rso1(ppstr);
        instr->print(rso1);
        std::string pstr = EscapeString(ppstr);

      for (User::op_iterator it = instr->op_begin(), e = instr->op_end(); it!=e; it++){
        Value *v = *it;
        Instruction *vi = dyn_cast<Instruction>(*it);

        std::string str;
        raw_string_ostream rso(str);
        vi->print(rso);
        std::string istr = EscapeString(str);
        if (istr.compare("")==0 or pstr.compare("")==0)
          continue;


        if (!obj.isSameWave(instr, vi))
          outs() << "\"" << pstr << "\"" << WA() << "\"" << istr << "\";\n";
        else
          outs() << "\"" << istr << "\" -> \"" << pstr << "\";\n";
      }
    }
    outs() << "}";
    */
    return false;
  }
};

char DataFlowGraph::ID = 0;
static RegisterPass<DataFlowGraph> X("dot-dataflow",
                                     "Output Dataflow Graph in DOT format for WaveScalar Architecture",
                                     false, false);
