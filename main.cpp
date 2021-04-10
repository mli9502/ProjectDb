#include "projectdb/projectdb.h"

int main() {
    projectdb::ProjectDb db{
        "/home/mli/COMSW_4995_Design_Using_C++/ProjectDb/config/"
        "config.template"};
    db.set("Hello", "World!");
    return 0;
}
