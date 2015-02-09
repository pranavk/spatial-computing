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

#include <string>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <iostream>
#include <map>

using namespace llvm;

void WaveScalar::runBFS(){
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

void WaveScalar::annotateWaves(const Function &F,
                               SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res){
    waveNo = 0;
    init(F);
    setBackEdges(F, res);
    for (Function::const_iterator I = F.begin(), IE = F.end(); I != IE; I++){
      ColorMap[I] = WaveScalar::WHITE;
    }

    Q.push_back(&F.getEntryBlock());
    runBFS();

  }

int WaveScalar::isSameWave(Instruction* i1,
                           Instruction *i2){
    std::string ppstr;
    raw_string_ostream rso1(ppstr);
    i1->print(rso1);

    std::string str;
    raw_string_ostream rso(str);
    i2->print(rso);

    return IMap[str] == IMap[ppstr];
}

int WaveScalar::isSameWave(std::string i1,
                           std::string i2){
  return IMap[i1] == IMap[i2];
}

int WaveScalar::getWaveNo(std::string instruction){
  return IMap[instruction];
}


void WaveScalar::init (const Function &F){
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

void WaveScalar::setIMap(BasicBlock* bb, unsigned waveNo){
    for (BasicBlock::iterator I = bb->begin(), IE = bb->end(); I!=IE; I++){
      Instruction *instr = &*I;
      std::string ppstr;
      raw_string_ostream rso1(ppstr);
      instr->print(rso1);

      IMap[ppstr] = waveNo;
    }
  }

void WaveScalar::setBackEdges (const Function &F, SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res){
    FindFunctionBackedges(F, *res);
    while (!res->empty()){
      std::pair<const BasicBlock*, const BasicBlock*> tempPair = res->pop_back_val();
      BEMap[tempPair.first] = 0;
      BEMap[tempPair.second] = 0;
    }
  }

void WaveScalar::setLabel(BasicBlock **succ, unsigned k){
    Twine::Twine twine(k);
    (*succ)->setName(twine);
  }

/*
  Helper Functions
*/
std::string EscapeString(const std::string &Label) {
  std::string Str(Label);
  for (unsigned i = 0; i != Str.length(); ++i)
    switch (Str[i]) {
    case '\n':
      Str.insert(Str.begin()+i, '\\');  // Escape character...
      ++i;
      Str[i] = 'n';
      break;
    case '\t':
      Str.insert(Str.begin()+i, ' ');  // Convert to two spaces
      ++i;
      Str[i] = ' ';
      break;
    case '\\':
      if (i+1 != Str.length())
        switch (Str[i+1]) {
          case 'l': continue; // don't disturb \l
          case '|': case '{': case '}':
            Str.erase(Str.begin()+i); continue;
          default: break;
        }
    case '{': case '}':
    case '<': case '>':
    case '|': case '"':
    case '[': case ']':
    case '%':
      Str.insert(Str.begin()+i, '\\');  // Escape character...
      ++i;  // don't infinite loop
      break;
    }
  return Str;
}

std::string insertWA(){
  return " -> WA -> ";
}
