KernelDB
========

KernelDB is an open source high performance persistent key-[JSON](http://json.org/) object store.  KernelDB supports pluggable stores backed by user specified kernels that are coded in JavaScript.


About KernelDB
--------------

KernelDB is powered by:
* [RocksDB](http://rocksdb.org/), an embeded database engine from Facebook (forked from Googles' [LevelDB](https://code.google.com/p/leveldb/)).
* [V8 JavaScript engine](https://code.google.com/p/v8/) from Google.


Supported Operators
-------------------

* *Get(key):* Returns object
* *Scan(key):* Forward and reverse iteration over keys in lexicographic order
* *Merge(key):* Atomic Read-Modify-Write operations defiend by __user supplied kernels!__
* *Put(key):* Adds/overwrites the value of an object
* *Delete(key):* Removes object from store


Pluggable
---------

A store is created with a JavaScript kernel that defines the semantic behavior of Merge operations.  For example, data being pushed into a key can have many different meanings:
* Incrementing a counter
* Maintaining a set
* LRU cache
* Maintaining complex state of visitors across a site

