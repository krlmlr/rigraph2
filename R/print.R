print.header <- function(object) {

  if (!is_igraph(object)) {
    stop("Not a graph object")
  }

  vc <- .Call(`C_R_igraph_vcount`, object)
}