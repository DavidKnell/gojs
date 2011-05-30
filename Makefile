# Copyright 2009 The Go Authors.  All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

include $(GOROOT)/src/Make.inc

TARG=gojs

# Can have plain GOFILES too, but this example doesn't.

CGOFILES=gojs.go

CGO_OFILES=c-gojs.o

CGO_CFLAGS=-I/usr/local/include/js 
CGO_LDFLAGS=-L/usr/local/lib -lmozjs185  -ldl  -lm -ldl

include $(GOROOT)/src/Make.pkg

hello:	install hello.go
	$(GC) hello.go
	$(LD) -o $@ hello.$O

