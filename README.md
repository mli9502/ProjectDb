# Course project for COMSW 4995: Design Using C++

# Doc for discussion
https://docs.google.com/document/d/1hUqSPuXYsKvIoN_2vQ-LpulPxyBMh52uJJ3slTLZsnc/edit

# Contributing to the repo
- Install `clang-format-11`.
- Run `./init.sh` to install pre-commit hook for formatting the code.

# Running Docker
Build the Docker using "docker build ."
Execute programs in the Docker using "docker run --rm -it -v `pwd`:/home/prodb -w=/home/prodb [image id]"

# References
- pre-commit hook for clang-format: https://github.com/barisione/clang-format-hooks/
- paper for tech used by nosql: https://www.cs.utexas.edu/~rossbach/cs378h/papers/nosql-survey.pdf
- LevelDb: The database used at a single node of BigTable: https://github.com/google/leveldb
- Google big table: http://static.googleusercontent.com/media/research.google.com/en//archive/bigtable-osdi06.pdf
- Designing Data-Intensive Applications: p76 - p79
- https://thispointer.com//c11-multithreading-part-9-stdasync-tutorial-example/

