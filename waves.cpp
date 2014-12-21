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
  void runDFS(const Function &F){
    outs() << "Running DFS on function : " << F.getName() << "\n";

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
    
    recursiveDFS (&F.getEntryBlock());
  }

  void runBFS(){
    while(!Q.empty()){
      BasicBlock *bb = const_cast<BasicBlock*>(Q.front());
      Q.pop_front();

      outs() << "Block " << IDMap[bb] << " is in Wave " << waveNo << "\n";
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
private:
  enum Color {WHITE, GREY, BLACK};
  typedef DenseMap<const BasicBlock *, Color> BBColorMap;
  typedef SmallVector<const BasicBlock *, 32> BBVector;
  typedef DenseMap<const BasicBlock *, unsigned> BBIDMap;
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

  /*
   This function is used to detect backedges in CFG. 
   */
  bool recursiveDFS(const BasicBlock *BB){
    ColorMap[BB] = WaveScalar::GREY;
    const TerminatorInst *TInst = BB->getTerminator();

    for (unsigned i = 0, nsucc = TInst->getNumSuccessors(); i < nsucc; i++){
      BasicBlock *succ = TInst->getSuccessor(i);
      Color succColor = ColorMap[succ];
      if (succColor == WaveScalar::WHITE){
        if (!recursiveDFS(succ))
          return false;
      }else if (succColor == WaveScalar::GREY) {
        outs() << "Detected cycle: from vertex ";
        outs() << IDMap[BB];
        outs() << " to " << IDMap[succ] << "\n";
        /*
          BEMap is initially set to 1 and it changes to 0 for all those nodes
          where backedge is detected.
        */
        BEMap[succ] = 0;
        BEMap[BB] = 0;
        return false;
      }
    }
    ColorMap[BB] = WaveScalar::BLACK;
    return true;
  } // start function
  
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
