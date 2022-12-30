## -----------------------------------------------------------------

#' A graph with no edges
#'
#' @aliases graph.empty
#' @concept Empty graph.
#' @param n Number of vertices.
#' @param directed Whether to create a directed graph.
#' @return An igraph graph.
#'
#' @family deterministic constructors
#' @export
#' @examples
#' make_empty_graph(n = 10)
#' make_empty_graph(n = 5, directed = FALSE)

make_empty_graph <- function(n=0, directed=TRUE) {
  # Argument checks
  if (is.null(n)) {
    stop("number of vertices must be an integer")
  }

  n <- suppressWarnings(as.integer(n))
  if (is.na(n)) {
    stop("number of vertices must be an integer")
  }

  directed <- as.logical(directed)
  
  on.exit( .Call(`C_R_igraph_finalizer`) )
  # Function call
  res <- .Call(`C_R_igraph_empty`, n, directed)

  res
}