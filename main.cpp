#include <iostream>
#include <cstdio>
#include <string>

#include "vector.hpp"
#include "account.hpp"
#include "utils.hpp"

int main() {
  //std::ios::sync_with_stdio(false);
  //std::cin.tie(nullptr);
  //std::cout.tie(nullptr);
  freopen("1.in", "r", stdin);
  freopen("1.out", "w", stdout);
  sjtu::account acc;
  std::string timestamp, cmd;
  while(std::cin >> timestamp) {
    std::cin >> cmd;
    if(cmd == "add_user") {
      std::string op;
      sjtu::Str<21> c;
      sjtu::Str<21> u;
      sjtu::Str<31> p;
      sjtu::Str<16> n;
      sjtu::Str<31> m;
      short g;
      for(int i = 0; i < 6; ++i) {
        std::cin >> op;
        if(op == "-c") std::cin >> c;
        else if(op == "-u") std::cin >> u;
        else if(op == "-p") std::cin >> p;
        else if(op == "-n") std::cin >> n;
        else if(op == "-m") std::cin >> m;
        else if(op == "-g") std::cin >> g;
      }
      if(acc.is_new()) {
        g = 10;
        acc.add_user(u, sjtu::user(u, p, n, m, g));
        std::cout << timestamp << " 0\n";
      }
      else {
        short cur_g = acc.find_cur_privilege(c);
        if(cur_g <= g) {
          std::cout << timestamp << " -1\n";
        }
        else {
          if(acc.add_user(u, sjtu::user(u, p, n, m, g))) {
            std::cout << timestamp << " 0\n";
          }
          else {
            std::cout << timestamp << " -1\n";
          }
        }
      }
    }
    else if(cmd == "login") {
      std::string op;
      sjtu::Str<21> u;
      sjtu::Str<31> p;
      for(int i = 0; i < 2; ++i) {
        std::cin >> op;
        if(op == "-u") std::cin >> u;
        else if(op == "-p") std::cin >> p;
      }
      auto ret = acc.find_user(u);
      if(ret.first && ret.second.password == p && acc.find_cur_privilege(u) == -1) {
        std::cout << timestamp << " 0\n";
        acc.cur.push_back(ret.second);
      }
      else {
        std::cout << timestamp << " -1\n";
      }
    }
    else if(cmd == "logout") {
      sjtu::Str<21> u;
      std::cin >> u >> u;
      if(acc.erase_cur(u)) {
        std::cout << timestamp << " 0\n";
      }
      else {
        std::cout << timestamp << " -1\n";
      }
    }
    else if(cmd == "query_profile") {
      std::string op;
      sjtu::Str<21> c;
      sjtu::Str<21> u;
      for(int i = 0; i < 2; ++i) {
        std::cin >> op;
        if(op == "-c") std::cin >> c;
        else if(op == "-u") std::cin >> u;
      }
      short cur_g = acc.find_cur_privilege(c);
      auto ret = acc.find_user(u);
      if(!ret.first || (c != u && cur_g <= ret.second.privilege)) {
        std::cout << timestamp << " -1\n";
      }
      else {
        std::cout << timestamp << ' ' << ret.second.username << ' ' << ret.second.name
                  << ' ' << ret.second.email << ' ' << ret.second.privilege << '\n';
      }
    }
    else if(cmd == "modify_profile") {
      std::string list;
      std::cin.get(); // Eat blank
      std::getline(std::cin, list);
      std::istringstream input(list);
      std::string op;
      sjtu::Str<21> c;
      sjtu::Str<21> u;
      sjtu::Str<31> p;
      sjtu::Str<16> n;
      sjtu::Str<31> m;
      short g = -1;
      while(input >> op) {
        if(op == "-c") input >> c;
        else if(op == "-u") input >> u;
        else if(op == "-p") input >> p;
        else if(op == "-n") input >> n;
        else if(op == "-m") input >> m;
        else if(op == "-g") input >> g;
      }
      short cur_g = acc.find_cur_privilege(c);
      auto ret = acc.find_user(u);
      if(!ret.first || (c != u && cur_g <= ret.second.privilege)) {
        std::cout << timestamp << " -1\n";
      }
      else {
        sjtu::user updated = ret.second;
        if(g != -1) {
          if(cur_g <= g) { std::cout << timestamp << " -1\n"; continue; }
          updated.privilege = g;
        }
        if(p[0] != '\0') {
          updated.password = p;
        }
        if(n[0] != '\0') {
          updated.name = n;
        }
        if(m[0] != '\0') {
          updated.email = m;
        }
        acc.modify_user(u, ret.second, updated);
        std::cout << timestamp << ' ' << updated.username << ' ' << updated.name
                  << ' ' << updated.email << ' ' << updated.privilege << '\n';
      }
    }
    else if(cmd == "exit") {
      std::cout << timestamp << " bye\n";
      return 0;
    }
    /*else {
      std::getline(std::cin, cmd);
    }*/
  }
}