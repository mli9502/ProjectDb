## What would you like to produce

A key-value storage engine library utilitzing `SSTable` and `Log-Structured Merge-Tree (LSM-Tree)`.

We would like to using modern C++ to implement a key-value storage engine library providing `get(<key>)`, `set(<key>, <value>)` and `delete(<key>)` apis. 

The library can be used as the base of a distributed NoSQL database that require persistant datastore. 

The library is planned to include the following components:

- `MemTable`: An in-memory map that temporarily holds the key-value pair that user want to set through `set` api, and also the first place `get` will check to retrieve the value.
- `SSTable`: 

## How would you like to start

## Who will initially do what

## What will you eventually want to measure (quantify)

## Why do you think you can do it on this tight schedule
