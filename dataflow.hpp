/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2014 Pranav Kant
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 */

#ifndef SPATIAL_DATAFLOW_HPP
#define SPATIAL_DATAFLOW_HPP

#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>

using namespace llvm;

template<typename T>
class DFG{
  T p;
public:
  DFG(T t) : p(t) { }
  std::string getFunctionName(){
    return p->getName();
  }
  T operator*() {return p;}
};


namespace llvm {
  template<>
  struct DOTGraphTraits<DFG<Function*> > : public DefaultDOTGraphTraits{
    explicit DOTGraphTraits(bool isSimple=false) : DefaultDOTGraphTraits(isSimple){}

    static std::string getGraphName(DFG<Function*> F){
      return "DFG for function \'" + F.getFunctionName() + "\'";
    }

    static std::string getNodeLabel(Value *v, const DFG<Function*> &F){
      Instruction *instr = dyn_cast<Instruction>(v);
      std::string str;
      raw_string_ostream rso(str);
      instr->print(rso);

      return str;
    }
  };

  template<>
  struct GraphTraits<DFG<Function*> > {
    typedef Value NodeType;
    typedef Value::use_iterator ChildIteratorType;

    static NodeType *getEntryNode(Value *G){
      return G;
    }

    static inline ChildIteratorType child_begin(NodeType *N){
      return N->use_begin();
    }

    static inline ChildIteratorType child_end(NodeType *N){
      return N->use_end();
    }

    typedef inst_iterator nodes_iterator;

    static nodes_iterator nodes_begin(DFG<Function*> F){
      return inst_begin(*F);
    }

    static nodes_iterator nodes_end(DFG<Function*> F){
      return inst_end(*F);
    }
  };
}
#endif
