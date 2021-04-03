//
// Created by Mengwen Li (ml4643) on 4/2/2021.
//

#ifndef MAIN_PROJECTDB_H
#define MAIN_PROJECTDB_H

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace projectdb {

class ProjectDbImpl;

class ProjectDb {
   public:
    // TODO: @mli: Add ctor that takes in a config file.
    ProjectDb();

    std::optional<std::string> get(const std::string& key);
    void set(const std::string& key, const std::string& value);
    void remove(const std::string& key);

   private:
    std::unique_ptr<ProjectDbImpl> m_impl;
};

}  // namespace projectdb

#endif  // MAIN_PROJECTDB_H
