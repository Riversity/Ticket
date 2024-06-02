#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <iostream>
#include "utils.hpp"
#include "unequal_bptree.hpp"

namespace sjtu {
  class user {
  public:
    Str<21> username;
    Str<31> password;
    Str<16> name;
    Str<31> email;
    short privilege;
    user(Str<21> username_, Str<31> password_, Str<16> name_, Str<31> email_, short privilege_)
        : username(username_), password(password_), name(name_), email(email_), privilege(privilege_) {}
    user() = default;
    bool operator < (const user& other) const {
      return username < other.username;
    }
    bool operator == (const user& other) const {
      return username == other.username;
    }
  };
  class account {
  private:
    BPTree_unique<Str<21>, user, 195, 40> user_bpt;
  public:
    vector<user> cur;
    account() : user_bpt("account_index", "account_val") {}
    ~account() = default;
    bool is_new() const {
      return user_bpt.empty();
    }
    bool add_user(Str<21>& key, user u) {
      return user_bpt.insert({key, u});
    }
    std::pair<bool, user> find_user(Str<21>& key) {
      return user_bpt.find(key);
    }
    bool modify_user(Str<21>& key, user original, user updated) {
      user_bpt.erase({key, original});
      return user_bpt.insert({key, updated});
    }
    short find_cur_privilege(Str<21>& key) {
      for(auto i : cur) {
        if(i.username == key) return i.privilege;
      }
      return -1;
    }
    bool erase_cur(Str<21>& key) {
      for(auto it = cur.begin(); it != cur.end(); ++it) {
        if((*it).username == key) {
          cur.erase(it);
          return true;
        }
      }
      return false;
    }
  };
}

#endif
