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

#include <string>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <iostream>
#include <map>
#include <deque>

using namespace llvm;

class WaveScalar{
public:
  void runBFS(){
    while(!Q.empty()){
      BasicBlock *bb = const_cast<BasicBlock*>(Q.front());
      Q.pop_front();

      outs() << "Block " << IDMap[bb] << " is in Wave " << waveNo << "\n";
      setIMap(bb, waveNo);
      const TerminatorInst *TInst = bb->getTerminator();
      for (unsigned i = 0, nsucc = TInst->getNumSuccessors(); i < nsucc; i++){
        BasicBlock *succ = TInst->getSuccessor(i);
        Color succColor = ColorMap[succ];
        if (succColor == WaveScalar::WHITE){
          ColorMap[succ] = WaveScalar::GREY;
          Q.push_back(succ);
        }
      }
      if (BEMap[bb] == 0){
        outs() << "Wave advanced at " << IDMap[bb] << "\n";
        waveNo++;
      }
      ColorMap[bb] = WaveScalar::BLACK;
    }
  }

  void annotateWaves(const Function &F, SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res){
    waveNo = 0;
    init(F);
    setBackEdges(F, res);
    for (Function::const_iterator I = F.begin(), IE = F.end(); I != IE; I++){
      ColorMap[I] = WaveScalar::WHITE;
    }
    
    Q.push_back(&F.getEntryBlock());
    runBFS();
  }

  int isSameWave(const Instruction* i1, const Instruction *i2){
    return IMap[i1] == IMap[i2];
  }
  
private:
  enum Color {WHITE, GREY, BLACK};
  typedef DenseMap<const BasicBlock *, Color> BBColorMap;
  typedef SmallVector<const BasicBlock *, 32> BBVector;
  typedef DenseMap<const BasicBlock *, unsigned> BBIDMap;
  typedef DenseMap<const Instruction*, unsigned> InstrMap;
  InstrMap IMap;
  BBColorMap ColorMap;
  BBIDMap IDMap, BEMap;
  std::deque<const BasicBlock*> Q;
  unsigned waveNo;

  void init (const Function &F){
    //Intialize the color map.
    unsigned K = 0;
    for (Function::const_iterator I = F.begin(), IE = F.end(); I != IE; I++, K++){
      ColorMap[I] = WaveScalar::WHITE;
      IDMap[I] = K;
      const BasicBlock *pbb = &*I; 
      BasicBlock *pp = const_cast<BasicBlock*>(pbb);
      setLabel(&pp, K);
      BEMap[I] = 1;
    }
  }

  void setIMap(BasicBlock* bb, unsigned waveNo){
    for (BasicBlock::iterator I = bb->begin(), IE = bb->end(); I!=IE; I++){
      Instruction *instr = &*I;
      IMap[instr] = waveNo;
    }
  }
  
  void setBackEdges (const Function &F, SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res){
    FindFunctionBackedges(F, *res);
    while (!res->empty()){
      std::pair<const BasicBlock*, const BasicBlock*> tempPair = res->pop_back_val();
      BEMap[tempPair.first] = 0;
      BEMap[tempPair.second] = 0;
    }
  }
  
  void setLabel(BasicBlock **succ, unsigned k){
    std::stringstream ss;
    ss << k;
    std::string str = ss.str();
    (*succ)->setName(str);
  }
};

/*
  Wave class for waves pass. The main task of this pass is to annotate waves in
  a control flow graph.
*/
struct Waves : public FunctionPass, public SmallVectorImpl <std::pair<const BasicBlock*, const BasicBlock*> > {
  static char ID;
  Waves() : FunctionPass(ID), SmallVectorImpl(10){}

  bool runOnFunction(Function &F) {
    if (F.getName() == "main")
      return false;
    outs() << "Basic blocks of function " << F.getName() << "\n";

    SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res = this;
    WaveScalar obj;
    obj.annotateWaves(F, res);
    F.viewCFGOnly();

    
    return false;
  }
};

char Waves::ID = 0;
static RegisterPass<Waves> X("annotate", "Annotate Waves", false, false);