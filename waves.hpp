/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2014 Pranav Kant
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 */

#ifndef SPATIAL_WAVES_HPP
#define SPATIAL_WAVES_HPP

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
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
//#include "llvm/Support/GraphWriter.h"

#include <deque>

using namespace llvm;

class WaveScalar{
 public:
  void runBFS();
  void annotateWaves(const Function &F, SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res);
  int isSameWave (Instruction* i, Instruction* e);
  int getWaveNo(std::string instruction);
  int isSameWave (std::string i1, std::string i2);

 private:
  enum Color {WHITE, GREY, BLACK};
  typedef DenseMap<const BasicBlock *, Color> BBColorMap;
  typedef SmallVector<const BasicBlock *, 32> BBVector;
  typedef DenseMap<const BasicBlock *, unsigned> BBIDMap;
  //  typedef DenseMap<std::string, unsigned> InstrMap;
  //  InstrMap IMap;
  std::map<std::string, int> IMap;
  BBColorMap ColorMap;
  BBIDMap IDMap, BEMap;
  std::deque<const BasicBlock*> Q;
  unsigned waveNo;

  void init (const Function &F);
  void setIMap(BasicBlock* bb, unsigned waveNo);
  void setBackEdges (const Function &F, SmallVectorImpl<std::pair<const BasicBlock*, const BasicBlock*> > *res);
  void setLabel(BasicBlock **succ, unsigned k);
};

/*
  Helper Functions
*/
std::string escapeString (const std::string &Label);

std::string insertWA ();

#endif
