
kerneldbdir=$(sysconfdir)/kerneldb
kerneldb_DATA =         \
	kerneldb.json

kernelsdir=$(kerneldbdir)/kernels
kernels_DATA =         \
	kernels/counter.js  \
	kernels/executor.js \
	kernels/list.js

install-data-local:
	$(MKDIR_P) $(localstatedir)/kerneldb/meta
	$(MKDIR_P) $(localstatedir)/kerneldb/stores