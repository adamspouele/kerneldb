
kerneldbdir=$(sysconfdir)/kerneldb
kerneldb_DATA =   \
	kerneldb.json \
	defaults.json

kernelsdir=$(kerneldbdir)/kernels
kernels_DATA =          \
	kernels/counter.js  \
	kernels/executor.js \
	kernels/list.js     \
	kernels/put.js

install-data-local:
	$(MKDIR_P) $(localstatedir)/kerneldb/stores