#include "uuid/uuid.h"

#include <math.h>

#include "igraph_datatype.h"
#include "igraph_interface.h"
#include "igraph_vector.h"

#include "graphalt.h"

#define R_IGRAPH_TYPE_VERSION "0.8.0"
#define R_IGRAPH_VERSION_VAR ".__igraph_version__."

SEXP R_igraph2_warning(void)
{
  Rf_warning("hello world");
  return R_NilValue;
}

int R_SEXP_to_vector(SEXP sv, igraph_vector_int_t *v) {
  v->stor_begin=(int64_t*)REAL(sv);
  v->stor_end=v->stor_begin+GET_LENGTH(sv);
  v->end=v->stor_end;
  return 0;
}

SEXP R_igraph_finalizer(void) {
  IGRAPH_FINALLY_FREE();
  return R_NilValue;
}

SEXP R_igraph_add_env(SEXP graph) {
  SEXP result = graph;
  int i;
  uuid_t my_id;
  char my_id_chr[40];
  int px = 0;

  if (GET_LENGTH(graph) != 10) {
    PROTECT(result = NEW_LIST(10)); px++;
    for (i = 0; i < 9; i++) {
      SET_VECTOR_ELT(result, i, duplicate(VECTOR_ELT(graph, i)));
    }
    SET_ATTRIB(result, duplicate(ATTRIB(graph)));
    SET_CLASS(result, duplicate(GET_CLASS(graph)));
  }

  SET_VECTOR_ELT(result, 9, allocSExp(ENVSXP));

  uuid_generate(my_id);
  uuid_unparse_lower(my_id, my_id_chr);

  SEXP l1 = PROTECT(install("myid")); px++;
  SEXP l2 = PROTECT(mkString(my_id_chr)); px++;
  defineVar(l1, l2, VECTOR_ELT(result, 9));

  l1 = PROTECT(install(R_IGRAPH_VERSION_VAR)); px++;
  l2 = PROTECT(mkString(R_IGRAPH_TYPE_VERSION)); px++;
  defineVar(l1, l2, VECTOR_ELT(result, 9));

  UNPROTECT(px);

  return result;
}

SEXP R_igraph_empty(SEXP n, SEXP directed) {
                                        /* Declarations */
  igraph_t* c_graph;
  igraph_integer_t c_n;
  igraph_bool_t c_directed;
  SEXP graph = R_NilValue;
                                        /* Convert input */
  c_n=INTEGER(n)[0];
  c_directed=LOGICAL(directed)[0];
                                        /* Call igraph */
  c_graph = new igraph_t{};
  igraph_empty(c_graph, c_n, c_directed);

  graph = create_graph(c_graph);
  SET_CLASS(graph, ScalarString(CREATE_STRING_VECTOR("igraph2")));

  return graph;
}

SEXP R_igraph_vcount(SEXP graph) {
  igraph_t* c_graph = static_cast<igraph_t*>(R_ExternalPtrAddr(R_altrep_data1(graph)));
  igraph_integer_t c_result;
  SEXP r_result;

  c_result = igraph_vcount(c_graph);

  PROTECT(r_result=NEW_INTEGER(1));
  INTEGER(r_result)[0]=c_result;

  UNPROTECT(1);
  return(r_result);
}

static const R_CallMethodDef CallEntries[] = {
    {"R_igraph2_warning", (DL_FUNC) &R_igraph2_warning, 0},
    {"R_igraph_empty", (DL_FUNC) &R_igraph_empty, 2},
    {"R_igraph_finalizer", (DL_FUNC) &R_igraph_finalizer, 0},
    {"R_igraph_vcount", (DL_FUNC) &R_igraph_vcount, 1},

    {NULL, NULL, 0}
};

extern "C" void attribute_visible R_init_igraph2(DllInfo *dll) {
  register_graph(dll, "graph", "igraph2");
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}