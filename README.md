# `ProjectDb`

A key-value storage engine library

---

# Getting Started:

## Requirements

- `cmake version >= 3.16.*`
- `gcc version >= 10.2.0` (This is needed for proper c++20 support)

## Usage

To use `ProjectDb`, follow the following steps:

- Clone this repo into some directory (`<root>` for example)

- Run `make init_build && make projectdb`
  
    This will build a `libprojectdb.a` static library, which needs to be linked against the application that uses `ProjectDb`.

- Set the c++ standard with `-std=gnu++20`

- Add `<root>/include` to the include path, and `#include <projectdb/projectdb.h>` in places where `ProjectDb` is used

- Add `libprojectdb.a` to the linker path, and add `-lprojectdb -lpthread` in at the end of link line

## Example

The directory structure for this example is as following:
```
.
..
ProjectDbTest
    main.cpp .............> This is the application that's trying to use ProjectDb
ProjectDb ................> This is cloned ProjectDb repo 
    cmake-build-release
        libprojectdb.a ...> This is generated after running "make init_build && make projectdb"
```

`main.cpp`, which is the application, is as following:

```c++
// main.cpp

#include "projectdb/projectdb.h"

int main() {
    projectdb::ProjectDb db;
    db.set("Hello", "World!");
    return 0;
}
```

The command used to build `main.cpp` is as following:

```bash
g++ -std=gnu++20 -I../ProjectDb/include main.cpp -o main \
    -L../ProjectDb/cmake-build-release/ -lprojectdb -lpthread
```

## Current Limitations

- The current `ProjectDb` apis are __not__ thread safe by itself. `ProjectDb` object needs to be locked by user if the apis are called in a multi-threaded fashion. 

- The current implementation doesn't provide multi-process support. 

  It is important to note that __only one `ProjectDb` should be created for one `DB_FILE_PATH`__, otherwise, there's a risk of data corruption.

---

# Evaluating from Docker

- Build the Docker using `docker build .`

- Open an interactive `bash` with current directory mounted to `projectdb`: `docker run -it --rm -v ${PWD}:/projectdb projectdb bash`

---

# Docs:

All the documents can be found in [docs/](https://github.com/mli9502/ProjectDb/tree/main/docs), specifically:

- [`DesignDocument.pdf`](https://github.com/mli9502/ProjectDb/blob/main/docs/DesignDocument.pdf): Documents the description of the algorithm implemented, and some design decisions made while implementing.
- [`Document.pdf`](https://github.com/mli9502/ProjectDb/blob/main/docs/Document.pdf): Detailed document about the directory, files, classes and functions implemented.
- [`Measurements.pdf`](https://github.com/mli9502/ProjectDb/blob/main/docs/Measurements.pdf): Documentation about the benchmarks on the library.
- [`Presentation.pdf`](https://github.com/mli9502/ProjectDb/blob/main/docs/Presentation.pdf): The slides for the presentation for the project.
- [`ProjectProposal.pdf`](https://github.com/mli9502/ProjectDb/blob/main/docs/ProjectProposal.pdf): The initial proposal for this project.
- [`Tutorial.pdf`](https://github.com/mli9502/ProjectDb/blob/main/docs/Tutorial.pdf): Documents on how to use the library.

---

# References

- pre-commit hook for clang-format: https://github.com/barisione/clang-format-hooks/
- paper for tech used by nosql: https://www.cs.utexas.edu/~rossbach/cs378h/papers/nosql-survey.pdf
- LevelDb: The database used at a single node of BigTable: https://github.com/google/leveldb
- Google big table: http://static.googleusercontent.com/media/research.google.com/en//archive/bigtable-osdi06.pdf
- Designing Data-Intensive Applications: p76 - p79
- https://thispointer.com//c11-multithreading-part-9-stdasync-tutorial-example/

---

# Contributing to the repo

- Install `clang-format-11`.
- Run `./init.sh` to install pre-commit hook for formatting the code.


