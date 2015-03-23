/*
 * Spatial Computation - Manifestation of Spatial computing.
 * Copyright © 2015 Shashwat Pandey
 *
 * This code is available to you under Apache License Version 2.0, January
 * 2014. You can grab a copy of license from the same repository from where you
 * fetched this code.
 */

#include "base_node.hpp"

class Gamma : public BaseNode {
	bool P;
	int vtrue, vfalse;
	int out;

	void *P, *vtrue, *vfalse;
	void *out;

	Gamma() : BaseNode(3, 1, "gamma") {} 
}
