#include "utility.hpp"
#include <functional>
#include <cstddef>
#include <vector>
#include <map>
//#include "exception.hpp"

namespace sjtu {
    template <class Key, class Value>
    class BTree {
    private:
        // Your private members go here
        std::map<Key,Value> m;
    public:
        BTree() {

        }

        BTree(const char *fname) {

        }

        ~BTree() {

        }

        // Clear the BTree
        void clear() {
            m.clear();
        }

        bool insert(const Key &key, const Value &value) {
            m.insert(std::make_pair(key,value));
            return true;
        }

        bool modify(const Key &key, const Value &value) {
            m[key]=value;
            return true;
        }

        Value at(const Key &key) {
            if (m.find(key)!=m.end()) return m[key];else return Value();
        }

        bool erase(const Key &key) {
            return m.erase(key);
        }
        
        
        class iterator {
        private:
            
        public:
            iterator() {
                
            }
            iterator(const iterator& other) {
                
            }

            // modify by iterator
            bool modify(const Value& value) {
                
            }

            Key getKey() const {

            }

            Value getValue() const {

            }

            iterator operator++(int) {

            }

            iterator& operator++() {
                
            }
            iterator operator--(int) {
                
            }

            iterator& operator--() {
                
            }

            // Overloaded of operator '==' and '!='
            // Check whether the iterators are same
            bool operator==(const iterator& rhs) const {
                
            }

            bool operator!=(const iterator& rhs) const {
                
            }
        };
        
        typename std::map<Key,Value>::iterator begin() {

        }
        
        // return an iterator to the end(the next element after the last)
        typename std::map<Key,Value>::iterator end() {
            return m.end();
        }

        typename std::map<Key,Value>::iterator find(const Key &key) {
            return m.find(key);
        }
        
        // return an iterator whose key is the smallest key greater or equal than 'key'
        iterator lower_bound(const Key &key) {
            
        }
        size_t size(){
            return m.size();
        }
    };
}  // namespace sjtu

