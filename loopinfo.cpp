/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2015 Pranav Kant
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 */

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/InstIterator.h"

#include "llvm/Support/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Local.h"


#include "llvm/Analysis/MemoryDependenceAnalysis.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Support/ValueHandle.h"
#include "llvm/Support/GraphWriter.h"

using namespace llvm;
namespace {
  class FnArgCnt : public FunctionPass {
  public:
    static char ID;
    FnArgCnt() : FunctionPass(ID) {}

    virtual void getAnalysisUsage (AnalysisUsage &AU) const {
      AU.setPreservesAll();
      AU.addRequired<LoopInfo>();
    }

    virtual bool runOnFunction(Function &F) {
      LoopInfo &LI = getAnalysis<LoopInfo>();
      int k = 0;
      for (Function::iterator BBi = F.begin(), BBe = F.end(); BBi!=BBe; BBi++){
        BasicBlock *bb = &*BBi;
        bb->setName(Twine(k++));
      }

      for (inst_iterator I = inst_begin(F), E = inst_end(F); I!=E; ++I){
        Instruction *instr = &*I;

          PHINode *phi = dyn_cast<PHINode>(instr);
          outs() << *instr << "\n";
          for (User::op_iterator it = instr->op_begin(), e = instr->op_end(); it!=e; it++){
            Instruction *vi = dyn_cast<Instruction>(*it);
            if(vi == NULL) {
              outs () << "\t\tNULL\n" ;
              continue;
            }
            outs() << "\t\t" << *vi << "\n";

        }
      }
    
      outs() << "Only below basic blocks are Loop Headers : \n";
      for (Function::iterator BBi = F.begin(), BBe = F.end(); BBi!=BBe; BBi++){
        BasicBlock *bb = &*BBi;
        if(LI.isLoopHeader(bb)){
          outs() <<  "\t"<<bb->getName() << "\n";
        }
      }

      outs() << "LoopDepth : Block number\tLoopDepth \n";
      for (Function::iterator BBi = F.begin(), BBe = F.end(); BBi!=BBe; BBi++){
        BasicBlock *bb = &*BBi;
        unsigned depth = LI.getLoopDepth(bb);
        outs() <<  "\t\t"<<bb->getName() <<"\t\t\t" << depth<<"\n";
      }



      /*
      outs() << "getLoopfor - Returns the innermost loop that BB lives in \n";
      for (Function::iterator BBi = F.begin(), BBe = F.end(); BBi!=BBe; BBi++){
        BasicBlock *bb = &*BBi;
        Loop *loop = LI.getLoopFor(bb);
        outs() << "For BB : " << bb->getName() <<"\n";

      }
      */

      LoopInfoBase<BasicBlock, Loop> &libase = LI.getBase();
      libase.print(outs());

      /*for (Function::iterator BBi = F.begin(), BBe = F.end(); BBi!=BBe; BBi++){
        BasicBlock *bb = &*BBi;
        Loop *lo = libase.getLoopFor(bb);
        lo->getCanonicalInductionVariable();
      }
      */
      typedef LoopInfoBase<BasicBlock, Loop>::iterator liBaseIt;
      for (liBaseIt it = LI.begin(), it_end = LI.end(); it!=it_end; it++){
        Loop *loop = *it;
        unsigned k =loop->getNumBackEdges();
        outs() << k << "\n";
        if (loop->isAnnotatedParallel())
          outs() << "This is parallel\n";
        else
          outs() << "This is not parallel\n";

        

        /*        for (inst_iterator I = inst_begin(F), E = inst_end(F); I!=E; ++I){
          Instruction *instr = &*I;
          bool changed;
          if(loop->makeLoopInvariant(instr, changed)){
            outs() << "make loop invariant\n";
            std::string ppstr;
            raw_string_ostream rso1(ppstr);
            instr->print(rso1);

            outs() << ppstr << "\n";
          }
          }*/

        //PHINode *node = loop->getCanonicalInductionVariable();


      }


      outs() << "Outputting CFG ...\n";
      std::string ErrorInfo;
      raw_fd_ostream File1("cfg.dot", ErrorInfo);
      WriteGraph (File1, (const Function*)&F);

      return false;
    }
  };
}
char FnArgCnt::ID = 0;
static RegisterPass<FnArgCnt> X("loopinfo", "Extract loop info", false, false);
