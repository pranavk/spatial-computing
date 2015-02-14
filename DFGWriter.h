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
#include <vector>
#include <map>
#include <string>
#include "dataflow.hpp"
#include "waves.hpp"

namespace llvm {

class DFGWriter {
  raw_ostream &O;
  const DFG<Function*> &G;
  // steerMap[Input instruction][select pin instruction (cmp)];
  std::map<Instruction*, std::map<Instruction*, int> > steerMap;
  int steerNo;
  std::map<int, Instruction*> branchMap;
  std::map<Instruction*, const void*> branchToID;

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
public:
  DFGWriter(raw_ostream &o,
            const DFG<Function*> &g,
            bool SN,
            WaveScalar &obj,
            DominatorTree &dt) : O(o), G(g), wavescalar(obj), DT(dt) {
    DTraits = DOTTraits(SN);
    steerNo = 1;
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
    // Loop over the graph, printing it out...
    for (node_iterator I = GTraits::nodes_begin(G), E = GTraits::nodes_end(G);
         I != E; ++I)
      if (!isNodeHidden(*I))
        writeNode(*I);

    for (std::map<int, Instruction*>::iterator it = branchMap.begin(); it!=branchMap.end(); it++){
      O << "\tNode" << branchToID[it->second] << " -> " << "Node0s" << it->first << ":ne ;\n";
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

    std::string edgeSourceLabels;
    raw_string_ostream EdgeSourceLabels(edgeSourceLabels);
    bool hasEdgeSourceLabels = getEdgeSourceLabels(EdgeSourceLabels, Node);

    if (hasEdgeSourceLabels) {
      if (!DTraits.renderGraphFromBottomUp()) O << "|";

      O << "{" << EdgeSourceLabels.str() << "}";

      if (DTraits.renderGraphFromBottomUp()) O << "|";
    }

    if (DTraits.renderGraphFromBottomUp()) {
      O << DOT::EscapeString(DTraits.getNodeLabel(Node, G));

      // If we should include the address of the node in the label, do so now.
      if (DTraits.hasNodeAddressLabel(Node, G))
        O << "|" << static_cast<const void*>(Node);

      std::string NodeDesc = DTraits.getNodeDescription(Node, G);
      if (!NodeDesc.empty())
        O << "|" << DOT::EscapeString(NodeDesc);
    }

    if (DTraits.hasEdgeDestLabels()) {
      O << "|{";

      unsigned i = 0, e = DTraits.numEdgeDestLabels(Node);
      for (; i != e && i != 64; ++i) {
        if (i) O << "|";
        O << "<d" << i << ">"
          << DOT::EscapeString(DTraits.getEdgeDestLabel(Node, i));
      }

      if (i != e)
        O << "|<d64>truncated...";
      O << "}";
    }

    O << "}\"];\n";   // Finish printing the "node" line

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

    void *n1 = const_cast<void*>(SrcNodeID);
    void *n2 = const_cast<void*>(DestNodeID);

    NodeType *node1 = static_cast<NodeType*>(n1);
    NodeType *node2 = static_cast<NodeType*>(n2);

    Instruction *i1 = dyn_cast<Instruction>(node1);
    Instruction *i2 = dyn_cast<Instruction>(node2);

    if(isa<BranchInst>(i2)){
      return;
    }

    if(isa<CmpInst>(i2)){
      branchToID[i2] = DestNodeID;
    }

    BasicBlock *BB1 = i1->getParent();
    BranchInst *br1 = dyn_cast<BranchInst>(BB1->getTerminator());

    O << "\tNode" << SrcNodeID;
    if (SrcNodePort >= 0)
      O << ":s" << SrcNodePort;
    Instruction *ins;
    if (i2->getParent() != BB1 && br1->isConditional()){
      Value *con = br1->getCondition();
      ins = dyn_cast<Instruction>(con); //this is condition instruction.
      BasicBlock *bb1 = br1->getSuccessor(0);
      BasicBlock *bb2 = br1->getSuccessor(1);

      const BasicBlockEdge bbtedge (BB1, bb1);
      const BasicBlockEdge bbfedge (BB1, bb2);

      if (steerMap.find(i1) != steerMap.end() && steerMap[i1].find(ins) != steerMap[i1].end
())
        {
          int no = steerMap[i1][ins];
          if (i2->getParent() == bb1 || DT.dominates(bbtedge, i2->getParent())){
            // for steer true;
            O << " -> Node0s" << no << ":n;";
            O << "\n";
            O << "\t";
            O << "Node0s" << no << ":sw";
          }else if (i2->getParent() == bb2 || DT.dominates(bbfedge, i2->getParent())){
            // for steer false;
            O << " -> Node0s" << no << ":n;";
            O << "\n";
            O << "\t";
            O << "Node0s" << no << ":se";
          }
        }
      else
        {
          int no = steerNo++;
          steerMap[i1][ins] = no;
          isSteer = no;
          if (i2->getParent() == bb1 || DT.dominates(bbtedge, i2->getParent())){
            // for steer true;
            O << " -> Node0s" << no << ":n;";
            O << "\n";
            O << "\t";
            O << "Node0s" << no << ":sw";
          }else if (i2->getParent() == bb2 || DT.dominates(bbfedge, i2->getParent())){
            // for steer false;
            O << " -> Node0s" << no << ":n;";
            O << "\n";
            O << "\t";
            O << "Node0s" << no << ":se";
          }
        }

    }

    O << " -> Node" << DestNodeID;
    if (DestNodePort >= 0 && DTraits.hasEdgeDestLabels())
      O << ":d" << DestNodePort;

    if (!Attrs.empty())
      O << "[" << Attrs << "]";
    O << ";\n";


    if (isSteer){
      branchMap[isSteer] = ins;
    }

    // This writes the steer node definition to dot file. isSteer contains the
    // steer number of the node.
    writeSteerNode(isSteer);
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
                      bool ShortNames = false,
                      const Twine &Title = "") {
  // Start the graph emission process...
  DFGWriter W(O, G, ShortNames, wavescalar, DT);

  // Emit the graph.
  W.writeGraph(Title.str());

  return O;
}

} // End llvm namespace

#endif
