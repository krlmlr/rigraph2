#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/Visibility.h>

#include "uuid/uuid.h"

#include <math.h>

#include "igraph.h"
#include "igraph_datatype.h"
#include "igraph_interface.h"
#include "igraph_vector.h"

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

int R_SEXP_to_igraph(SEXP graph, igraph_t *res) {

  res->n=(igraph_integer_t) REAL(VECTOR_ELT(graph, 0))[0];
  res->directed=LOGICAL(VECTOR_ELT(graph, 1))[0];
  R_SEXP_to_vector(VECTOR_ELT(graph, 2), &res->from);
  R_SEXP_to_vector(VECTOR_ELT(graph, 3), &res->to);
  R_SEXP_to_vector(VECTOR_ELT(graph, 4), &res->oi);
  R_SEXP_to_vector(VECTOR_ELT(graph, 5), &res->ii);
  R_SEXP_to_vector(VECTOR_ELT(graph, 6), &res->os);
  R_SEXP_to_vector(VECTOR_ELT(graph, 7), &res->is);

  /* attributes */
  // REAL(VECTOR_ELT(VECTOR_ELT(graph, 8), 0))[0] = 1; /* R objects refcount */
  // REAL(VECTOR_ELT(VECTOR_ELT(graph, 8), 0))[1] = 0; /* igraph_t objects */
  //res->attr=VECTOR_ELT(graph, 8);

  return 0;
}

SEXP R_igraph_vcount(SEXP graph) {
                                        /* Declarations */
  igraph_t c_graph;
  igraph_integer_t c_result;
  SEXP r_result;
                                        /* Convert input */
  R_SEXP_to_igraph(graph, &c_graph);
                                        /* Call igraph */
  c_result=  igraph_vcount(&c_graph);

                                        /* Convert output */

  PROTECT(r_result=NEW_INTEGER(1));
  INTEGER(r_result)[0]=c_result;

  UNPROTECT(1);
  return(r_result);
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

SEXP R_igraph_to_SEXP(const igraph_t *graph) {

  SEXP result;
  long int no_of_nodes=igraph_vcount(graph);
  long int no_of_edges=igraph_ecount(graph);

  PROTECT(result=NEW_LIST(10));
  SET_VECTOR_ELT(result, 0, NEW_NUMERIC(1));
  SET_VECTOR_ELT(result, 1, NEW_LOGICAL(1));
  SET_VECTOR_ELT(result, 2, NEW_NUMERIC(no_of_edges));
  SET_VECTOR_ELT(result, 3, NEW_NUMERIC(no_of_edges));
  SET_VECTOR_ELT(result, 4, NEW_NUMERIC(no_of_edges));
  SET_VECTOR_ELT(result, 5, NEW_NUMERIC(no_of_edges));
  SET_VECTOR_ELT(result, 6, NEW_NUMERIC(no_of_nodes+1));
  SET_VECTOR_ELT(result, 7, NEW_NUMERIC(no_of_nodes+1));

  REAL(VECTOR_ELT(result, 0))[0]=no_of_nodes;
  LOGICAL(VECTOR_ELT(result, 1))[0]=graph->directed;
  memcpy(REAL(VECTOR_ELT(result, 2)), graph->from.stor_begin,
         sizeof(igraph_integer_t)*(size_t) no_of_edges);
  memcpy(REAL(VECTOR_ELT(result, 3)), graph->to.stor_begin,
         sizeof(igraph_integer_t)*(size_t) no_of_edges);
  memcpy(REAL(VECTOR_ELT(result, 4)), graph->oi.stor_begin,
         sizeof(igraph_integer_t)*(size_t) no_of_edges);
  memcpy(REAL(VECTOR_ELT(result, 5)), graph->ii.stor_begin,
         sizeof(igraph_integer_t)*(size_t) no_of_edges);
  memcpy(REAL(VECTOR_ELT(result, 6)), graph->os.stor_begin,
         sizeof(igraph_integer_t)*(size_t) (no_of_nodes+1));
  memcpy(REAL(VECTOR_ELT(result, 7)), graph->is.stor_begin,
         sizeof(igraph_integer_t)*(size_t) (no_of_nodes+1));

  SET_CLASS(result, ScalarString(CREATE_STRING_VECTOR("igraph2")));

  // /* Attributes */
  // SET_VECTOR_ELT(result, 8, (SEXP) graph->attr);
  // REAL(VECTOR_ELT((SEXP)graph->attr, 0))[0] += 1;

  // /* Environment for vertex/edge seqs */
  // SET_VECTOR_ELT(result, 9, R_NilValue);
  // R_igraph_add_env(result);

  UNPROTECT(1);
  return result;
}

SEXP R_igraph_empty(SEXP n, SEXP directed) {
                                        /* Declarations */
  igraph_t c_graph;
  igraph_integer_t c_n;
  igraph_bool_t c_directed;
  SEXP graph = R_NilValue;

  SEXP r_result;
                                        /* Convert input */
  c_n=INTEGER(n)[0];
  c_directed=LOGICAL(directed)[0];
                                        /* Call igraph */
  igraph_empty(&c_graph, c_n, c_directed);

                                        /* Convert output */
  IGRAPH_FINALLY(igraph_destroy, &c_graph);
  PROTECT(graph=R_igraph_to_SEXP(&c_graph));
  igraph_destroy(&c_graph);
  IGRAPH_FINALLY_CLEAN(1);
  r_result = graph;

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
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}