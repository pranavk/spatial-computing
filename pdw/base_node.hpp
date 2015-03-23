/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2015 Pranav Kant
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 */

#include <string>

class BaseNode
{
  int in_edges;
  int out_edges;
  std::string name;
public:
  BaseNode(int in_edges, int out_edges, std::string name) : in_edges(in_edges), out_edges(out_edges), name(name) {}
  int get_no_of_in_edges ();
  int get_no_of_out_edges ();
  int get_name ();
};
