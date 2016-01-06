# Build against mingw-w64 build of libsodium 1.0.3
if(!file.exists("../windows/rsvg-2.40.12/include/librsvg-2.0/librsvg/rsvg.h")){
  if(getRversion() < "3.3.0") setInternet2()
  download.file("https://github.com/rwinlib/rsvg/archive/v2.40.12.zip", "lib.zip", quiet = TRUE)
  dir.create("../windows", showWarnings = FALSE)
  unzip("lib.zip", exdir = "../windows")
  unlink("lib.zip")
}
