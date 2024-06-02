#include <iostream>
#include <cstdio>
#include <string>
#include <climits>
#include <cstddef>
#include <cstring>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cassert>

namespace sjtu {
  template <int len>
  class Str {
  private:
    char st[len];
  public:
    Str() {
      st[0] = '\0';
    }
    Str(const char* s) {
      strcpy(st, s);
    }
    Str(const std::string &s) {
      strcpy(st, s.c_str());
    }
    char &operator[](int pos) {
      return st[pos];
    }
    Str &operator=(const Str &rhs) {
      if(this == &rhs) return *this;
      strcpy(st, rhs.st);
      return *this;
    }
    Str &operator=(const char *s) {
      strcpy(st, s);
      return *this;
    }
    Str &operator=(const std::string &s) {
      strcpy(st, s.c_str());
      return *this;
    }
    bool operator==(const Str &rhs) const {
      return !strcmp(st, rhs.st);
    }
    bool operator!=(const Str &rhs) const {
      return strcmp(st, rhs.st);
    }
    bool operator<(const Str &rhs) const {
      return strcmp(st, rhs.st) < 0;
    }
    bool operator<=(const Str &rhs) const {
      return strcmp(st, rhs.st) <= 0;
    }
    friend std::istream &operator>>(std::istream &is, Str &rhs) {
      return is >> rhs.st;
    }
    friend std::ostream &operator<<(std::ostream &os, const Str &rhs) {
      return os << rhs.st;
    }
  };
}
namespace sjtu {

template<class T, int info_len>
class File {
private:
  //int tail = (info_len + 1) * sizeof(int);
  std::fstream file;
  std::string file_name;
  int siz_T = sizeof(T);

public:
  File() = delete;

  File(const std::string& file_name) : file_name(file_name) {}

  void initialise(std::string file_n = "") {
    if(file_n != "") file_name = file_n;
    file.open(file_name, std::ios::out);
    int tmp = 0;
    for(int i = 0; i <= info_len; ++i)
      file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  void get_info(int &tmp, int n = 1) {
    if (n > info_len) return;
    file.open(file_name, std::ios::in);
    file.seekg(n * sizeof(int));
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  void write_info(int tmp, int n = 1) {
    if (n > info_len) return;
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(n * sizeof(int));
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  int write(T &t) {
    file.open(file_name, std::ios::in | std::ios::out);
    int pos;
    file.read(reinterpret_cast<char *>(&pos), sizeof(int));
    if(pos == 0) {
      file.seekp(0, std::ios::end);
      int ret = file.tellp();
      file.write(reinterpret_cast<char *>(&t), siz_T);
      //tail = ret + siz_T;
      file.close();
      return ret;
    }
    else {
      file.seekg(pos);
      int next_pos;
      file.read(reinterpret_cast<char *>(&next_pos), sizeof(int));
      file.seekp(0);
      file.write(reinterpret_cast<char *>(&next_pos), sizeof(int));
      file.seekp(pos);
      file.write(reinterpret_cast<char *>(&t), siz_T);
      file.close();
      return pos;
    }
  }

  void update(T &t, const int index) {
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&t), siz_T);
    file.close();
  }

  void read(T &t, const int index) {
    file.open(file_name, std::ios::in);
    file.seekg(index);
    file.read(reinterpret_cast<char *>(&t), siz_T);
    file.close();
  }

  void del(int index) {
    file.open(file_name, std::ios::in | std::ios::out);
    file.seekg(0);
    int next_pos;
    file.read(reinterpret_cast<char *>(&next_pos), sizeof(int));
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&next_pos), sizeof(int));
    file.seekp(0);
    file.write(reinterpret_cast<char *>(&index), sizeof(int));
    file.close();
  }
};

}

namespace sjtu {
template<class Key, class T, const int M = 100, const int L = 100>
class BPTree_unique {
private:
  int total;
  int root; // pos in f_i
  using Pair = std::pair<Key, T>;

  struct index_node {
    Key val[M];
    int pos_index_node[M + 1]; // one more
    int cnt;
    bool isLeaf;
    index_node() {
      cnt = 0;
      isLeaf = true;
    }
    int unequal_higher_bound(const Key& origin) const { // find the one > origin
      if(cnt == 1) return pos_index_node[0];
      if(val[cnt - 2] <= origin) return pos_index_node[cnt - 1];
      int l = 0, r = cnt - 2;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m] <= origin) l = m + 1;
        else r = m;
      }
      return pos_index_node[l];
    }
    int higher_bound(const Key& origin) const { // find the one >= origin
      if(cnt == 1) return pos_index_node[0];
      if(val[cnt - 2] < origin) return pos_index_node[cnt - 1];
      int l = 0, r = cnt - 2;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m] < origin) l = m + 1;
        else r = m;
      }
      return pos_index_node[l];
    }
    std::pair<int, int> higher_bound_pair(const Key& origin) const { // find the one > origin
      if(cnt == 1) return {pos_index_node[0], 0};
      if(val[cnt - 2] <= origin) return {pos_index_node[cnt - 1], cnt - 1};
      int l = 0, r = cnt - 2;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m] <= origin) l = m + 1;
        else r = m;
      }
      return {pos_index_node[l], l};
    }
    /*
    int higher_bound_by_key(const Key& origin) const { // find the one >= origin
      if(cnt == 1) return pos_index_node[0];
      if(val[cnt - 2].first < origin) return pos_index_node[cnt - 1];
      int l = 0, r = cnt - 2;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m].first < origin) l = m + 1;
        else r = m;
      }
      return pos_index_node[l];
    }
    */
    void insert_index(const Key& dat, int pos, int k) {
      // pos_index_node[k] is splitted
      for(int i = cnt - 1; i > k; --i) {
        val[i] = val[i - 1];
        pos_index_node[i + 1] = pos_index_node[i];
      }
      val[k] = dat;
      pos_index_node[k + 1] = pos;
      ++cnt;
    }
    void delete_by_pos(int k) {
      for(int i = k; i < cnt - 1; ++i) {
        pos_index_node[i] = pos_index_node[i + 1];
        val[i - 1] = val[i];
      }
      --cnt;
    }
  };
  struct val_node { // leaf node
    Pair val[L + 1];
    int siz;
    int nxt_pos;
    val_node() {
      siz = 0;
      nxt_pos = -1;
    }
    int higher_bound(const Pair& origin) const { // find the one >= origin
      if(siz == 0 || val[siz - 1] < origin) return siz;
      int l = 0, r = siz - 1;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m] < origin) l = m + 1;
        else r = m;
      }
      return l;
    }
    int real_higher_bound_by_key(const Key& origin) const { // find the one >= origin
      if(siz == 0 || val[siz - 1].first < origin) return siz;
      int l = 0, r = siz - 1;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m].first < origin) l = m + 1;
        else r = m;
      }
      return l;
    }
    int higher_bound_by_key(const Key& origin) const { // find the one >= origin
      if(siz == 0 || val[siz - 1].first < origin) return -1; // Here specialized
      int l = 0, r = siz - 1;
      while(l < r) {
        int m = (l + r) >> 1;
        if(val[m].first < origin) l = m + 1;
        else r = m;
      }
      return l;
    }
    bool insert(const Pair& origin) {
      //siz < L
      int hi_bound = real_higher_bound_by_key(origin.first);
      if(hi_bound != siz && origin.first == val[hi_bound].first) return false;
      // notice, no repetition guaranteed here
      for(int i = siz; i > hi_bound; --i) {
        val[i] = val[i - 1];
      }
      val[hi_bound] = origin;
      ++siz;
      return true;
    }
    bool erase(const Pair& origin) {
      if(siz == 0) return false;
      int hi_bound = higher_bound(origin);
      if(hi_bound == siz || origin != val[hi_bound]) return false;
      for(int i = hi_bound + 1; i < siz; ++i) {
        val[i - 1] = val[i];
      }
      --siz;
      return true;
    }
  };

  File<index_node, 2> f_index; // 1 for total, 2 for root
  File<val_node, 2> f_val;

public:
  explicit BPTree_unique(const std::string &name_f_index, const std::string &name_f_val) 
                                        : f_index(name_f_index), f_val(name_f_val) {
    //std::fstream file;
    //file.open(name_f_index, std::ios::in);
    if(!std::filesystem::exists(name_f_index)) {
      f_index.initialise();
      f_val.initialise();
      total = 0;
      root = -1;
    }
    else {
      f_index.get_info(total, 1);
      f_index.get_info(root, 2);
    }
  }

  ~BPTree_unique() {
    f_index.write_info(total, 1);
    f_index.write_info(root, 2);
  }

  int find_first(int index_node_pos, const Key &key) { // Return pos of the block in f_val
    index_node r;
    f_index.read(r, index_node_pos);
    if(r.isLeaf) {
      return r.unequal_higher_bound(key);
    }
    else {
      int nxt_pos = r.unequal_higher_bound(key);
      return find_first(nxt_pos, key);
    }
  } // Notice, not necessarily find the one, maybe the preceding one.
  std::pair<bool, T> find(const Key &key) {
    if(root == -1) return {false, T()};
    int pos = find_first(root, key);
    val_node block;
    f_val.read(block, pos);
    int k = block.higher_bound_by_key(key);

    if(k == -1 || block.val[k].first != key) {
      return {false, T()};
    }
    return {true, block.val[k].second};
  }

  std::pair<bool, int> val_insert(const Pair& dat, int pos, Key& ret, bool& isSuccess) {
    val_node cur;
    f_val.read(cur, pos);
    if(cur.siz == L) {
      // L-L/2 to original, L/2 to new
      val_node new_cur;
      new_cur.nxt_pos = cur.nxt_pos;
      for(int i = 0; i < L/2; ++i) {
        new_cur.val[i] = cur.val[i + L-L/2];
      }
      cur.siz = L-L/2;
      new_cur.siz = L/2;
      if(new_cur.val[0] <= dat) isSuccess = new_cur.insert(dat);
      else isSuccess = cur.insert(dat);
      ret = new_cur.val[0].first;
      int new_cur_pos = f_val.write(new_cur);
      cur.nxt_pos = new_cur_pos;
      f_val.update(cur, pos);
      return {true, new_cur_pos};
    }
    else {
      if(cur.insert(dat)) {
        ++total;
        f_val.update(cur, pos);
      }
      else isSuccess = false;
      return {false, 0};
    }
  }
  
  std::pair<bool, int> insert_at(const Pair& dat, int index_pos, Key& ret, bool& isSuccess) {
    // return value is status: whether splitted, where new block
    index_node cur;
    f_index.read(cur, index_pos);

    std::pair<int, int> pos = cur.higher_bound_pair(dat.first);

    std::pair<bool, int> res;
    Key new_ret;
    if(cur.isLeaf) {
      res = val_insert(dat, pos.first, new_ret, isSuccess);
    }
    else {
      res = insert_at(dat, pos.first, new_ret, isSuccess);
    }
    if(res.first) {
      // splitted, node + 1
      if(cur.cnt == M) {
        // M-M/2 to original, M/2+1 to new
        cur.insert_index(new_ret, res.second, pos.second);
        index_node new_cur;
        int new_cur_pos;
        for(int i = 0; i < M/2; ++i) {
          new_cur.pos_index_node[i] = cur.pos_index_node[M-M/2 + i];
          new_cur.val[i] = cur.val[M-M/2 + i];
        }
        new_cur.pos_index_node[M/2] = cur.pos_index_node[M];
        ret = cur.val[M-M/2 - 1];
        new_cur.cnt = M/2 + 1;
        cur.cnt = M-M/2;
        new_cur.isLeaf = cur.isLeaf;
        f_index.update(cur, index_pos);
        new_cur_pos = f_index.write(new_cur);
        if(index_pos == root) {
          index_node new_root;
          new_root.isLeaf = false;
          new_root.cnt = 2;
          new_root.val[0] = ret;
          new_root.pos_index_node[0] = index_pos;
          new_root.pos_index_node[1] = new_cur_pos;
          root = f_index.write(new_root);
        }
        return {true, new_cur_pos};
      }
      else {
        cur.insert_index(new_ret, res.second, pos.second);
        f_index.update(cur, index_pos);
        return {false, 0};
      }
    }
    else return {false, 0};
  }
  bool insert(const Pair& dat) {
    if(root == -1) {
      val_node v;
      v.val[0] = dat;
      v.siz = 1;
      index_node i;
      i.isLeaf = true;
      i.cnt = 1;
      i.pos_index_node[0] = f_val.write(v);
      root = f_index.write(i);
      return true;
    }
    Key ret;
    bool isSuccess = true;
    insert_at(dat, root, ret, isSuccess);
    return isSuccess;
  }
  std::pair<bool, int> erase_at(const Pair& dat, int index_pos, bool& isSuccess) {
    // first return value whether erased, second the ordinal of the deleted node
    index_node r;
    f_index.read(r, index_pos);
    std::pair<int, int> pos_right = r.higher_bound_pair(dat.first);
    int pos_left = -1;
    if(pos_right.second > 0) pos_left = r.pos_index_node[pos_right.second - 1];
    if(r.isLeaf) {
      val_node v_right;
      f_val.read(v_right, pos_right.first);
      if(v_right.erase(dat)) {
        --total;
        if(pos_left != -1) {
          val_node v_left;
          f_val.read(v_left, pos_left);
          if(v_left.siz + v_right.siz <= L) { // merge
            for(int i = 0; i < v_right.siz; ++i) {
              v_left.val[v_left.siz + i] = v_right.val[i];
            }
            v_left.siz += v_right.siz;
            v_left.nxt_pos = v_right.nxt_pos;
            f_val.del(pos_right.first);
            f_val.update(v_left, pos_left);
            return {true, pos_right.second};
          }
        }
        f_val.update(v_right, pos_right.first);
      }
      else isSuccess = false;
      return {false, -1};
    }
    else {
      std::pair<bool, int> ret = erase_at(dat, pos_right.first, isSuccess);
      if(ret.first) {
        index_node i_right;
        f_index.read(i_right, pos_right.first);
        i_right.delete_by_pos(ret.second);
        if(pos_left != -1) {
          index_node i_left;
          f_index.read(i_left, pos_left);
          if(i_left.cnt + i_right.cnt < M) {
            if(i_right.cnt > 0) {
              for(int i = 0; i < i_right.cnt - 1; ++i) {
                i_left.pos_index_node[i_left.cnt + i] = i_right.pos_index_node[i];
                i_left.val[i_left.cnt + i] = i_right.val[i];
              }
              i_left.pos_index_node[i_left.cnt + i_right.cnt - 1] = i_right.pos_index_node[i_right.cnt - 1];
              i_left.val[i_left.cnt - 1] = r.val[pos_right.second - 1];
              i_left.cnt += i_right.cnt;
            }
            f_index.del(pos_right.first);
            f_index.update(i_left, pos_left);
            return {true, pos_right.second};
          }
        }
        f_index.update(i_right, pos_right.first);
      }
      return {false, -1};
    }
  }
  bool erase(const Pair& dat) {
    if(root == -1) return false;
    bool isSuccess = true;
    std::pair<bool, int> ret = erase_at(dat, root, isSuccess);
    if(ret.first) {
      index_node i_right;
      f_index.read(i_right, root);
      i_right.delete_by_pos(ret.second);
      f_index.update(i_right, root);
    }
    return isSuccess;
  }
  bool empty() const {
    return root == -1;
  }
  /*
  void put(int pos) {
    val_node s;
    f_val.read(s, pos);
    std::cerr<<"Now block at "<<pos<<std::endl;
    std::cerr<<"Block size is "<<s.siz<<std::endl;
    for(int i = 0; i < s.siz; ++i) {
      std::cerr<<s.val[i].first<<s.val[i].second<<std::endl;
    }
    std::cerr<<"Block End!"<<std::endl;
  }
  void traverse(int pos) {
    index_node r;
    f_index.read(r, pos);
    std::cerr<<"Now index at "<<pos<<std::endl;
    std::cerr<<"Index size is "<<r.cnt<<std::endl;
    if(r.isLeaf) {
      for(int i = 0; i < r.cnt - 1; ++i) {
        put(r.pos_index_node[i]);
        std::cerr<<"Pivot is "<<r.val[i]<<std::endl;
      }
      put(r.pos_index_node[r.cnt - 1]);
    }
    else {
      for(int i = 0; i < r.cnt - 1; ++i) {
        traverse(r.pos_index_node[i]);
        std::cerr<<"Pivot is "<<r.val[i]<<std::endl;
      }
      traverse(r.pos_index_node[r.cnt - 1]);
    }
    std::cerr<<"Index "<<pos<<" End!"<<std::endl;
  }
  void traverse() {
    traverse(root);
  }
  void test(int m) {
    const char* kk = "aaaa";
    Key k(kk);
    for(int i = 0; i < m; ++i) {
      insert({k, i});
      erase({k, i-1});
    }
    auto v = find(k);
    for(int i : v) {
      std::cout<<i<<std::endl;
    }
  }
  */
};

}

namespace sjtu {

class exception {
protected:
	const std::string variant = "";
	std::string detail = "";
public:
	exception() {}
	exception(const exception &ec) : variant(ec.variant), detail(ec.detail) {}
	virtual std::string what() {
		return variant + " " + detail;
	}
};

class index_out_of_bound : public exception {
	/* __________________________ */
};

class runtime_error : public exception {
	/* __________________________ */
};

class invalid_iterator : public exception {
	/* __________________________ */
};

class container_is_empty : public exception {
	/* __________________________ */
};
}
namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
auto resize_func = [] (size_t s) { return s / 2 * 3 + 1; };
template<typename T>
class vector
{
private:
  size_t capacity, siz;
  T* data;

public:
  class const_iterator;
  class iterator
  {
    friend class vector;
  // The following code is written for the C++ type_traits library.
  // Type traits is a C++ feature for describing certain properties of a type.
  // For instance, for an iterator, iterator::value_type is the type that the 
  // iterator points to. 
  // STL algorithms and containers may use these type_traits (e.g. the following 
  // typedef) to work properly. In particular, without the following code, 
  // @code{std::sort(iter, iter1);} would not compile.
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::output_iterator_tag;

  private:
    vector* origin;
    int pos;
  public:
    iterator(vector* _origin = nullptr, int _pos = 0)
            : origin (_origin), pos (_pos) {}
    ~iterator() = default;
    iterator(iterator &other) {
      origin = other.origin;
      pos = other.pos;
    }
    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    iterator operator+(const int &n) const 
    {
      return iterator(origin, pos + n);
    }
    iterator operator-(const int &n) const 
    {
      return iterator(origin, pos - n);
    }
    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw invaild_iterator.
    int operator-(const iterator &rhs) const 
    {
      if(origin != rhs.origin) throw invalid_iterator();
      else return pos - rhs.pos;
    }
    iterator& operator+=(const int &n) 
    {
      pos += n;
      return *this;
    }
    iterator& operator-=(const int &n) 
    {
      pos -= n;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp(*this);
      ++pos;
      return tmp;
    }
    iterator& operator++() {
      ++pos;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp(*this);
      --pos;
      return tmp;
    }
    iterator& operator--() {
      --pos;
      return *this;
    }
    T& operator*() const{
      return origin->data[pos];
    }
    /**
     * a operator to check whether two iterators are same (pointing to the same memory address).
     */
    bool operator==(const iterator &rhs) const {
      return (origin == rhs.origin) && (pos == rhs.pos);
    }
    bool operator==(const const_iterator &rhs) const {
      return (origin == rhs.origin) && (pos == rhs.pos);
    }
    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
  };

  class const_iterator 
  {
    friend class vector;

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::output_iterator_tag;

  private:
    const vector* origin;
    int pos;

  public:
    const_iterator(const vector* _origin = nullptr, int _pos = 0)
            : origin(_origin), pos(_pos) {}
    ~const_iterator() = default;
    const_iterator(const const_iterator &other) {
      origin = other.origin;
      pos = other.pos;
    }
    const_iterator operator+(const int &n) const 
    {
      return const_iterator(origin, pos + n);
    }
    const_iterator operator-(const int &n) const 
    {
      return const_iterator(origin, pos - n);
    }
    int operator-(const const_iterator &rhs) const 
    {
      if(origin != rhs.origin) throw invalid_iterator();
      else return pos - rhs.pos;
    }
    const_iterator& operator+=(const int &n) 
    {
      pos += n;
      return *this;
    }
    const_iterator& operator-=(const int &n) 
    {
      pos -= n;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator tmp(*this);
      ++pos;
      return tmp;
    }
    const_iterator& operator++() {
      ++pos;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator tmp(*this);
      --pos;
      return tmp;
    }
    const_iterator& operator--() {
      --pos;
      return *this;
    }
    const T& operator*() const {
      return origin->data[pos];
    }
    bool operator==(const const_iterator &rhs) const {
      return (origin == rhs.origin) && (pos == rhs.pos);
    }
    bool operator==(const iterator &rhs) const {
      return (origin == rhs.origin) && (pos == rhs.pos);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
  };

  vector() : capacity(16), siz(0){
    data = (T*) malloc(capacity * sizeof(T));
  }
  vector(const vector &other) : capacity(other.siz), siz(other.siz) {
    data = (T*) malloc(capacity * sizeof(T));
    for(int i = 0; i < siz; ++i) {
      new(data + i) T(other[i]); // Placement new
    }
  }

  ~vector() {
    destroy();
  }
  void destroy() {
    for(int i = 0; i < siz; ++i) {
      data[i].~T();
    }
    free(data);
  }

  vector &operator=(const vector &other) {
    if(this != &other) {
      destroy();
      capacity = other.capacity, siz = other.siz;
      data = (T*) malloc(capacity * sizeof(T));
      for(int i = 0; i < siz; ++i) {
        new(data + i) T(other[i]);
      }
    }
    return *this;
  }
  // Move the whole chunk to a bigger space
  void expand() {
    capacity = resize_func(capacity); // siz maintained
    T* n_data = (T*) malloc(capacity * sizeof(T));
    for(int i = 0; i < siz; ++i) {
      new(n_data + i) T(data[i]);
    }
    destroy();
    data = n_data;
  }

  T & at(const size_t &pos) {
    if(pos >= siz) {
      throw index_out_of_bound();
    }
    return data[pos];
  }
  const T & at(const size_t &pos) const {
    if(pos >= siz) {
      throw index_out_of_bound();
    }
    return data[pos];
  }
  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   */
  T & operator[](const size_t &pos) {
    return at(pos);
  }
  const T & operator[](const size_t &pos) const {
    return at(pos);
  }
  /**
   * access the first element.
   * throw container_is_empty if size == 0
   */
  const T & front() const {
    if(siz == 0) throw container_is_empty();
    return data[0];
  }
  const T & back() const {
    if(siz == 0) throw container_is_empty();
    return data[siz - 1];
  }
  /**
   * returns an iterator to the beginning.
   */
  iterator begin() {
    return iterator(this, 0);
  }
  const_iterator cbegin() const {
    return const_iterator(this, 0);
  }
  /**
   * returns an iterator to the end.
   */
  iterator end() {
    return iterator(this, siz);
  }
  const_iterator cend() const {
    return const_iterator(this, siz);
  }
  /**
   *
   */
  bool empty() const {
    return siz == 0;
  }
  size_t size() const {
    return siz;
  }
  void clear() {
    destroy();
    siz = 0; capacity = 16;
    data = (T*) malloc(capacity * sizeof(T));
  }
  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {
    if(siz == capacity) expand();
    new(data + siz) T(data[siz - 1]);
    for(int i = siz - 1; i > pos.pos; --i) {
      data[i] = data[i - 1];
    }
    data[pos.pos] = value;
    ++siz;
    return pos;
  }
  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {
    if(ind > siz) throw index_out_of_bound();
    if(siz == capacity) expand();
    new(data + siz) T(data[siz - 1]);
    for(int i = siz - 1; i > ind; --i) {
      data[i] = data[i - 1];
    }
    data[ind] = value;
    ++siz;
    return iterator(this, ind);
  }
  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is returned.
   */
  iterator erase(iterator pos) {
    for(int i = pos.pos; i < siz - 1; ++i) {
      data[i] = data[i + 1];
    }
    data[siz - 1].~T();
    --siz;
    return pos;
  }
  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {
    if(ind >= siz) throw index_out_of_bound();
    for(int i = ind; i < siz - 1; ++i) {
      data[i] = data[i + 1];
    }
    data[siz - 1].~T();
    --siz;
    return iterator(this, ind);
  }
  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {
    if(siz == capacity) expand();
    new(data + siz) T(value);
    ++siz;
  }
  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {
    if(siz == 0) throw container_is_empty();
    data[siz - 1].~T();
    --siz;
  }
};
}


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