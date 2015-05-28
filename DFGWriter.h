/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2014 Pranav Kant
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 *
 * Below code is based on GraphWriter.h in LLVM source tree.
 */

#ifndef LLVM_SUPPORT_DFGWRITER_H
#define LLVM_SUPPORT_DFGWRITER_H

#include "llvm/ADT/GraphTraits.h"
#include "llvm/Support/DOTGraphTraits.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include <cassert>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include "dataflow.hpp"
#include "waves.hpp"

typedef std::map<int, Instruction*> steerPinMap_t;
typedef std::map<Instruction*, const void*> condToIDMap_t;
typedef std::map<Instruction*, int> phi_gamma_t;
typedef std::map<Instruction*, int> beta_t;
typedef std::map<Instruction*, int> alpha_t;

typedef struct NodePred{
  int sno;
  Instruction *predicate;
} NodePred_t;

namespace llvm {

class DFGWriter {
  raw_ostream &O;
  const DFG<Function*> &G;
  //instruction to beta_alpha pair.
  std::map<Instruction*, int> instTobaMap;
  std::map<Instruction*, std::string> instToIDMap;

  steerPinMap_t steerPinMap;
  phi_gamma_t phi_gamma;
  beta_t beta;
  alpha_t alpha;

  int baID;
  int beta_id;
  int alpha_id;
  int phi_gamma_id;
  int delta_id;

  typedef DOTGraphTraits<DFG<Function*> >     DOTTraits;
  typedef GraphTraits<DFG<Function*> >        GTraits;
  typedef GTraits::NodeType          NodeType;
  typedef GTraits::nodes_iterator    node_iterator;
  typedef GTraits::ChildIteratorType child_iterator;
  DOTTraits DTraits;

  // Writes the edge labels of the node to O and returns true if there are any
  // edge labels not equal to the empty string "".
  bool getEdgeSourceLabels(raw_ostream &O, NodeType *Node) {
    child_iterator EI = GTraits::child_begin(Node);
    child_iterator EE = GTraits::child_end(Node);
    bool hasEdgeSourceLabels = false;

    for (unsigned i = 0; EI != EE && i != 64; ++EI, ++i) {
      std::string label = DTraits.getEdgeSourceLabel(Node, EI);

      if (label.empty())
        continue;

      hasEdgeSourceLabels = true;

      if (i)
        O << "|";

      O << "<s" << i << ">" << DOT::EscapeString(label);
    }

    if (EI != EE && hasEdgeSourceLabels)
      O << "|<s64>truncated...";

    return hasEdgeSourceLabels;
  }
  WaveScalar &wavescalar;
  DominatorTree &DT;
  LoopInfo &LI;
public:
  DFGWriter(raw_ostream &o,
            const DFG<Function*> &g,
            bool SN,
            WaveScalar &obj,
            DominatorTree &dt,
            LoopInfo &li) : O(o), G(g), wavescalar(obj), DT(dt), LI(li)  {
    DTraits = DOTTraits(SN);
    phi_gamma_id = 1;
    delta_id = 1;
    beta_id=1;
    alpha_id =1;
  }

  /*
    Finds the predicate by reverse iterating for given phi node.
    This algorithm is usually used when instr = PHI node.
   */
  Instruction *findPredicate_revIterate(Instruction *instr) {
    BasicBlock *BB = instr->getParent();
    BasicBlock *bb = BB;

    while ((bb = bb->getSinglePredecessor()) != NULL) {

    }

  }


  /*
    Finds the predicate instruction, if any, inside the basic block containing instr.
   */
  Instruction *findPredicate_sameBlock(Instruction *instr) {
    BasicBlock *BB = instr->getParent();
    Instruction *predicate = NULL;
    for(BasicBlock::iterator i = BB->begin(), ie= BB->end(); i!=ie; ++i ){
      Instruction *inst = &*i;
      if(isa<BranchInst>(inst)){
        BranchInst *br = dyn_cast<BranchInst>(inst);
        if(br->isConditional()){
          Value *condition = br->getCondition();
          predicate = dyn_cast<Instruction>(condition);
          return predicate;
        }

      }

    }// for end
    return predicate;
  }

  void writeGraph(const std::string &Title = "") {
    // Output the header for the graph...
    writeHeader(Title);

    // Emit all of the nodes in the graph...
    writeNodes();

    // Output any customizations on the graph
    DOTGraphTraits<DFG<Function*> >::addCustomGraphFeatures(G, *this);

    // Output the end of the graph
    writeFooter();
  }

  void writeHeader(const std::string &Title) {
    std::string GraphName = DTraits.getGraphName(G);

    if (!Title.empty())
      O << "strict digraph \"" << DOT::EscapeString(Title) << "\" {\n";
    else if (!GraphName.empty())
      O << "strict digraph \"" << DOT::EscapeString(GraphName) << "\" {\n";
    else
      O << "strict digraph unnamed {\n";

    if (DTraits.renderGraphFromBottomUp())
      O << "\trankdir=\"BT\";\n";

    if (!Title.empty())
      O << "\tlabel=\"" << DOT::EscapeString(Title) << "\";\n";
    else if (!GraphName.empty())
      O << "\tlabel=\"" << DOT::EscapeString(GraphName) << "\";\n";
    O << DTraits.getGraphProperties(G);
    O << "\n";
  }

  void writeFooter() {
    // Finish off the graph
    O << "}\n";
  }

  void writeNodes() {
    for (node_iterator I = GTraits::nodes_begin(G), E = GTraits::nodes_end(G);
         I != E; ++I){
      Instruction *inst = &*I;
      std::stringstream ss;
      ss << inst;
      std::string str = ss.str();
      instToIDMap[inst] = str;
    }

    // Loop over the graph, printing it out...
    for (node_iterator I = GTraits::nodes_begin(G), E = GTraits::nodes_end(G);
         I != E; ++I){
      if (!isNodeHidden(*I)){
        writeNode(*I);
      }
    }

  }

  bool isNodeHidden(NodeType &Node) {
    return isNodeHidden(&Node);
  }

  bool isNodeHidden(NodeType *const *Node) {
    return isNodeHidden(*Node);
  }

  bool isNodeHidden(NodeType *Node) {
    return DTraits.isNodeHidden(Node);
  }

  void writeNode(NodeType& Node) {
    writeNode(&Node);
  }

  void writeNode(NodeType *const *Node) {
    writeNode(*Node);
  }

  void writeNode(NodeType *Node) {
    std::string NodeAttributes = DTraits.getNodeAttributes(Node, G);

    Instruction *i1 = dyn_cast<Instruction>(Node);
    if(isa<BranchInst>(i1))
      return;

    if(!isa<PHINode>(i1)){
      O << "\tNode" << static_cast<const void*>(Node) << " [shape=record,";
      if (!NodeAttributes.empty()) O << NodeAttributes << ",";
      O << "label=\"{";


      if (!DTraits.renderGraphFromBottomUp()) {
        O << DOT::EscapeString(DTraits.getNodeLabel(Node, G));

        // If we should include the address of the node in the label, do so now.
        if (DTraits.hasNodeAddressLabel(Node, G))
          O << "|" << static_cast<const void*>(Node);

        std::string NodeDesc = DTraits.getNodeDescription(Node, G);
        if (!NodeDesc.empty())
          O << "|" << DOT::EscapeString(NodeDesc);
      }
          O << "}\"];\n";   // Finish printing the "node" line
    }




    // Output all of the edges now
    child_iterator EI = GTraits::child_begin(Node);
    child_iterator EE = GTraits::child_end(Node);
    for (unsigned i = 0; EI != EE && i != 64; ++EI, ++i)
      if (!DTraits.isNodeHidden(*EI))
        writeEdge(Node, i, EI);
    for (; EI != EE; ++EI)
      if (!DTraits.isNodeHidden(*EI))
        writeEdge(Node, 64, EI);
  }

  void writeSteerNode(int sNo) {
    if (!sNo)
      return;
    O << "\tNode0s" << sNo << " [shape=triangle,";
    O << "label=\"";
    O << "Steer";
    O << "\"];\n";   // Finish printing the "node" line
  }

  void writeBetaNode(NodePred_t stru) {
    int sNo = stru.sno;
    Instruction* predicate = stru.predicate;

    if(!sNo)
      return;
    O << "\tNode0b" << sNo << " [shape=rectangle,";
    O << "label=\"";
    O << "Beta";
    O << "\"];\n";   // Finish printing the "node" line

    //feed the predicate:
    O << "\tNode" << instToIDMap[predicate] << " -> ";
    O << "Node0b" << sNo << ":w;\n";
  }

  void writeAlphaNode(NodePred_t stru) {
    int sNo = stru.sno;
    Instruction* predicate = stru.predicate;
    if(!sNo)
      return;
    O << "\tNode0a" << sNo << " [shape=rectangle,";
    O << "label=\"";
    O << "Alpha";
    O << "\"];\n";   // Finish printing the "node" line

    //feed the predicate:
    O << "\tNode" << instToIDMap[predicate] << " -> ";
    O << "Node0a" << sNo << ":w;\n";
  }
  
  void writeBetaAlphaNode(int sNo) {
    if (!sNo)
      return;
    O << "\tNode0b" << sNo << " [shape=rectangle,";
    O << "label=\"";
    O << "Beta";
    O << "\"];\n";   // Finish printing the "node" line

    O << "\tNode0a" << sNo << " [shape=rectangle,";
    O << "label=\"";
    O << "Alpha";
    O << "\"];\n";   // Finish printing the "node" line
  }

  void writeGammaNode(int sNo, Instruction *predicate){
    if (!sNo)
      return;
    O << "\tNode0g" << sNo << " [shape=rectangle," ;
    O << "label=\"";
    O << "Gamma";
    O << "\"];\n"; //finish printing the node.

    //feed the predicate:
    O << "\tNode" << instToIDMap[predicate] << " -> ";
    O << "Node0g" << sNo << ":w;\n";
  }

  void writeDeltaNode(int sNo, Instruction *predicate){
    if (!sNo)
      return;
    O << "\tNode0d" << sNo << " [shape=rectangle," ;
    O << "label=\"";
    O << "Delta";
    O << "\"];\n"; //finish printing the node.

    //feed the predicate:
    O << "\tNode" << instToIDMap[predicate] << " -> ";
    O << "Node0d" << sNo << ":w;\n";
  }


  void writeEdge(NodeType *Node, unsigned edgeidx, child_iterator EI) {
    if (NodeType *TargetNode = *EI) {
      int DestPort = -1;
      if (DTraits.edgeTargetsEdgeSource(Node, EI)) {
        child_iterator TargetIt = DTraits.getEdgeTarget(Node, EI);

        // Figure out which edge this targets...
        unsigned Offset =
          (unsigned)std::distance(GTraits::child_begin(TargetNode), TargetIt);
        DestPort = static_cast<int>(Offset);
      }

      if (DTraits.getEdgeSourceLabel(Node, EI).empty())
        edgeidx = -1;

      emitEdge(static_cast<const void*>(Node), edgeidx,
               static_cast<const void*>(TargetNode), DestPort,
               DTraits.getEdgeAttributes(Node, EI, G));
    }
  }

  /// emitSimpleNode - Outputs a simple (non-record) node
  void emitSimpleNode(const void *ID, const std::string &Attr,
                      const std::string &Label, unsigned NumEdgeSources = 0,
                      const std::vector<std::string> *EdgeSourceLabels = 0) {
    O << "\tNode" << ID << "[ ";
    if (!Attr.empty())
      O << Attr << ",";
    O << " label =\"";
    if (NumEdgeSources) O << "{";
    O << DOT::EscapeString(Label);
    if (NumEdgeSources) {
      O << "|{";

      for (unsigned i = 0; i != NumEdgeSources; ++i) {
        if (i) O << "|";
        O << "<s" << i << ">";
        if (EdgeSourceLabels) O << DOT::EscapeString((*EdgeSourceLabels)[i]);
      }
      O << "}}";
    }
    O << "\"];\n";
  }



  /// emitEdge - Output an edge from a simple node into the graph...
  void emitEdge(const void *SrcNodeID, int SrcNodePort,
                const void *DestNodeID, int DestNodePort,
                const std::string &Attrs) {
    if (SrcNodePort  > 64) return;             // Eminating from truncated part?
    if (DestNodePort > 64) DestNodePort = 64;  // Targeting the truncated part?

    int isSteer = 0;

    //    O << "\tNode" << SrcNodeID;
    
    void *n1 = const_cast<void*>(SrcNodeID);
    void *n2 = const_cast<void*>(DestNodeID);

    NodeType *node1 = static_cast<NodeType*>(n1);
    NodeType *node2 = static_cast<NodeType*>(n2);

    Instruction *i1 = dyn_cast<Instruction>(node1);
    Instruction *i2 = dyn_cast<Instruction>(node2);

    if (isa<BranchInst>(i2))
      return;
    
    BasicBlock *BB1 = i1->getParent();
    BasicBlock *BB2 = i2->getParent();

    // This gets the innermost loop for basicBlock BB. Returns null when no loop.
    Loop *loop1 = LI.getLoopFor(BB1);
    Loop *loop2 = LI.getLoopFor(BB2);

    /*
      For conditional blocks:
      getLoopDepth is used to check if both the instructions, i1 and i2 are
      outside the loop or within the same loop.
     */
    unsigned loopDepth_i1 = LI.getLoopDepth(BB1);
    unsigned loopDepth_i2 = LI.getLoopDepth(BB2);

    int writeGamma = 0;
    int writeBeta = 0;
    int writeAlpha = 0;
    int writeBetaAlpha = 0;
    Instruction *predicate = NULL;

    NodePred_t alphaStruct;
    NodePred_t betaStruct;

    alphaStruct.sno = 0;
    betaStruct.sno = 0;
    
    /*
    if(loop1 == loop2){
      // This implies that both the instructions are in the same loop or both
      // are outside the loop.

      if (isa<PHINode>(i2))
        {
          // This PHINode will be converted to Gamma node anyways.
          int phi_gamma_id_local;
          if(phi_gamma.find(i2) != phi_gamma.end())
            phi_gamma_id_local = phi_gamma[i2];
          else{
            phi_gamma[i2] = phi_gamma_id++;
            phi_gamma_id_local = phi_gamma[i2];
            writeGamma = phi_gamma_id_local;

            // also feed the predicate.
            // TODO : complete this predicate function used below.
            predicate = findPredicate_revIterate(i2);
          }

          // assumption: 0 is gamma true and 1 is gamma false.
          Value *phi0 = i2->getOperand(0);
          Value *phi1 = i2->getOperand(1);

          if(phi0 == i1){
            // attaching to true of gamma node
            O << " -> Node0g" << phi_gamma_id_local << ":nw;\n";
            O << "\tNode0g" << phi_gamma_id_local << ":s";
          }else{
            //when i1 is phi node's second argument;
            // attaching to true of gamma node.
            O << " -> Node0g" << phi_gamma_id_local << ":ne;\n";
            O << "\tNode0g" << phi_gamma_id_local << ":s";
          }
        }
      else // else for: isa<PHINode>(i2); i.e i2 is not a PHINode.
        {
          // start iterating from i1 to i2.
          Instruction *tmp_i1 = i1;

          while (!isa<BranchInst>(tmp_i1)) {
            BasicBlock::iterator it1(tmp_i1);
            it1++;
            tmp_i1 = it1;
          }

          BasicBlock *BB_tmp = tmp_i1->getParent();
          unsigned loopDepth_tmp = LI.getLoopDepth(BB_tmp);
          TerminatorInst *terminator_tmp = BB_tmp->getTerminator();
          if (loopDepth_tmp > loopDepth_i1) // means that block is part of the loop.
            {
              // assuming that we would have only two successors (true, false) at
              // a time.
              BasicBlock *bb = terminator_tmp->getSuccessor(1);
              tmp_i1 = bb->getFirstNonPHI();
            }
          else //it must be the branch instruction for some condition
            {
              BasicBlock *bb0 = terminator_tmp->getSuccessor(0);
              BasicBlock *bb1 = terminator_tmp->getSuccessor(1);

              BranchInst *br_tmp = dyn_cast<BranchInst>(tmp_i1);
              Value *val_tmp = br_tmp->getCondition();
              Instruction *predicate = dyn_cast<Instruction>(val_tmp);

              const BasicBlockEdge bbtedge(BB_tmp, bb0);
              const BasicBlockEdge bbfedge(BB_tmp, bb1);

              if (DT.dominates(bbtedge, i2->getParent())){

              }else if (DT.dominates(bbfedge, i2->getParent())){

              }else{

              }

            } //end else

        } //else isa<PHINode> (i2)
    }// fi loop1 == loop2

    else // else for: loop1 == loop2
    */
    if (1)// this is when either i1,i2 is inside loop.
      {
        /*
          This means that one among i1 and i2 are inside the loop body, and
          hence a special case according to algorithm.
         */
        if (loopDepth_i2 > loopDepth_i1){
          if (isa<PHINode>(i2)){
            beta[i2] = beta_id;
            writeBeta = beta_id;
            beta_id++;

            betaStruct.sno = writeBeta;

            O << "\tNode" << SrcNodeID;

            O << " -> Node0b" << writeBeta << ":nw;\n";
          }else{
            beta[i2] = beta_id;
            writeBeta = beta_id;
            beta_id++;

            betaStruct.sno = writeBeta;
            
            O << "\tNode" << SrcNodeID;
            
            O << " -> Node0b" << writeBeta << ":nw;\n";
            O << "\tNode0b" << writeBeta << ":s -> Node0b" << writeBeta << ":ne;\n";
            O << "\tNode0b" << writeBeta << ":s";

            O << " -> Node" << DestNodeID << ";\n";
          }
          //store the predicate; it will be feeded at the end
          betaStruct.predicate = findPredicate_sameBlock(i2);
        }
        else if (LI.isLoopHeader(BB1) and LI.isLoopHeader(BB2)){
          if (isa<PHINode>(i1)){
            if (alpha.find(i1) == alpha.end()){
              alpha[i1] = alpha_id;
              writeAlpha = alpha_id;
              alpha_id++;
              alphaStruct.sno = writeAlpha;
              alphaStruct.predicate = findPredicate_sameBlock(i1);
            }

            O << "\tNode0a" << alpha[i1] << ":s -> Node" << DestNodeID << ";\n";
            
          } else {
            O << "\tNode" << SrcNodeID;
            O << " -> Node" << DestNodeID << ";\n";
          }
        }
        else if (LI.isLoopHeader(BB1) and isa<PHINode>(i1)){
          if (loop2 != NULL){
            alpha[i1] = alpha_id;
            writeAlpha = alpha_id;
            alpha_id++;

            alphaStruct.sno = writeAlpha;

            if (beta.find(i1) == beta.end()) {
              beta[i1] = beta_id;
              writeBeta = beta_id;
              beta_id++;
              betaStruct.sno = writeBeta;
              betaStruct.predicate = findPredicate_sameBlock(i1);
            }
            
            O << "\tNode0b" << beta[i1] << ":s -> " <<  "Node0a" << writeAlpha << ":n;\n";
            O << "\tNode0a" << writeAlpha << ":sw -> " << "Node" << DestNodeID << ";\n";

            alphaStruct.predicate = findPredicate_sameBlock(i1);
          }else{
            alpha[i1] = alpha_id;
            writeAlpha = alpha_id;
            alpha_id++;

            alphaStruct.sno = writeAlpha;

            if (beta.find(i1) == beta.end()) {
              beta[i1] = beta_id;
              writeBeta = beta_id;
              beta_id++;
              betaStruct.sno = writeBeta;
              betaStruct.predicate = findPredicate_sameBlock(i1);
            }
            
            O << "\tNode0b" << beta[i1] << ":s -> " << "Node0a"<< writeAlpha <<":n;\n";
            O << "\tNode0a" << writeAlpha << ":se -> " << "Node" << DestNodeID << ";\n";

            alphaStruct.predicate = findPredicate_sameBlock(i1);
          }
        }
        else if (LI.isLoopHeader(BB1)){
          if (loop2 != NULL){
            alpha[i1] = alpha_id;
            writeAlpha = alpha_id;
            alpha_id++;

            alphaStruct.sno = writeAlpha;

            O << "\tNode" << SrcNodeID;

            O << " -> Node0a" << writeAlpha << ":n;\n";
            O << "\tNode0a" << writeAlpha << ":sw -> " << "Node" << DestNodeID << ";\n";

            alphaStruct.predicate = findPredicate_sameBlock(i1);
          }else {
            int tmp_alpha = alpha[i1];
            O << "\tNode0a" << tmp_alpha << ":se -> " << DestNodeID << ";\n";
          }
          
        }
        else if (loop1 != NULL and isa<PHINode>(i2)){
          O << "\tNode" << SrcNodeID << " -> Node0b" << beta[i2] << ":ne;\n";
        }

      }

    //    O << " -> Node" << DestNodeID << ";\n";

    writeGammaNode(writeGamma, predicate);
    writeBetaNode(betaStruct);
    writeAlphaNode(alphaStruct);
    //writeBetaAlphaNode(isba);
  }

  /// getOStream - Get the raw output stream into the graph file. Useful to
  /// write fancy things using addCustomGraphFeatures().
  raw_ostream &getOStream() {
    return O;
  }
};

raw_ostream &WriteDFG(raw_ostream &O,
                      const DFG<Function*> &G,
                      WaveScalar &wavescalar,
                      DominatorTree& DT,
                      LoopInfo& LI,
                      bool ShortNames = false,
                      const Twine &Title = "") {
  // Start the graph emission process...
  DFGWriter W(O, G, ShortNames, wavescalar, DT, LI);

  // Emit the graph.
  W.writeGraph(Title.str());

  return O;
}

} // End llvm namespace

#endif
