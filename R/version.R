#' librsvg version
#'
#' Print the version of the librsvg library
#'
#' @export
#' @useDynLib rsvg R_librsvg_version
librsvg_version <- function(){
  .Call(R_librsvg_version)
}

.onAttach <- function(libname, pkg){
  packageStartupMessage(paste("Linking to librsvg", librsvg_version()))
}
