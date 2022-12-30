test_that("sample test", {
  igraph2_warning()

  g <- make_empty_graph(n = 5)

  if (!is_igraph(g)) {
    stop('Not a graph object')
  }

  print.header(g)

})