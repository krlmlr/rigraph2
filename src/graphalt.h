#pragma once

#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/Visibility.h>
#include <R_ext/Altrep.h>

#include "igraph.h"

void register_graph(DllInfo* dll, const char* class_name, const char* package_name);

SEXP create_graph(igraph_t* graph);