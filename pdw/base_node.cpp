/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2015 Pranav Kant
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 */

#include "base_node.hpp"

int BaseNode::get_no_of_in_edges()
{
  return in_edges;
}

int BaseNode::get_no_of_out_edges()
{
  return out_edges;
}

std::string BaseNode::get_name()
{
  return name;
}
