#include "graphalt.h"

static R_altrep_class_t class_t;

void register_graph(DllInfo* dll, const char* class_name, const char* package_name) {
  class_t = R_make_altreal_class(class_name, package_name, dll);
}

static void delete_graph(SEXP xp){
  auto* c_graph = static_cast<igraph_t*>(R_ExternalPtrAddr(xp));
  igraph_destroy(c_graph);
  IGRAPH_FINALLY_CLEAN(1);
  delete c_graph;
}

SEXP create_graph(igraph_t* graph) {
  SEXP xp = PROTECT(R_MakeExternalPtr(graph, R_NilValue, R_NilValue));

  R_RegisterCFinalizerEx(xp, delete_graph, TRUE);
  SEXP res = R_new_altrep(class_t, xp, R_NilValue);
  UNPROTECT(1);

  return res;
}