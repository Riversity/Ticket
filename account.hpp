#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include "utils.hpp"

namespace sjtu {
  class user {
    Str<21> username;
    Str<31> password;
    Str<16> name;
    Str<31> email;
    short privilege;
  };
}

#endif
