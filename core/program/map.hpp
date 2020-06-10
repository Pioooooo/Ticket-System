/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "../btree/utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
    class Key,
    class T,
    class Compare = std::less<Key>
> class map {
public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
    typedef pair<const Key, T> value_type;
private:
    enum {RED,BLACK};
    struct node{
        value_type value;
        bool color;
        node *fa,*l,*r;
        //node():color(RED),fa(nullptr),l(nullptr),r(nullptr){}
        node (const value_type &x):value(x),color(RED),fa(nullptr),l(nullptr),r(nullptr){}
        node *gfa(){
            if (fa==nullptr) return nullptr;
            return fa->fa;
        }
        node *uncle(){
            if (gfa()==nullptr) return nullptr;
            if (gfa()->l==fa) return gfa()->r;else return gfa()->l;
        }
        node *brother(){
            if (fa==nullptr) return nullptr;
            if (this==fa->l) return fa->r;else return fa->l;
        }
    };
    node *root,*End;size_t sz;
    void del(node *x){
        if (x==nullptr) return;
        del(x->l);del(x->r);
        delete x;
    }
    void left_rotate(node *x){
        node *fa=x->fa,*y=x->r;
        x->r=y->l;if (y->l) y->l->fa=x;
        y->l=x;x->fa=y;
        y->fa=fa;
        if (fa){if (fa->l==x) fa->l=y;else fa->r=y;}else root=y;
    }
    void right_rotate(node *x){
        node *fa=x->fa,*y=x->l;
        x->l=y->r;if (y->r) y->r->fa=x;
        y->r=x;x->fa=y;
        y->fa=fa;
        if (fa){if (fa->l==x) fa->l=y;else fa->r=y;}else root=y;
    }
public:
    /**
     * see BidirectionalIterator at CppReference for help.
     *
     * if there is anything wrong throw invalid_iterator.
     *     like it = map.begin(); --it;
     *       or it = map.end(); ++end();
     */
    class const_iterator;
    class iterator {
        friend class map;
        friend class const_iterator;
    private:
        /**
         * TODO add data members
         *   just add whatever you want.
         */
        map *a;node *x;
    public:
        iterator():a(nullptr),x(nullptr){}
        iterator(const iterator &other):a(other.a),x(other.x){}
        iterator(map *aa,node *xx):a(aa),x(xx){}
        /**
         * return a new iterator which pointer n-next elements
         *   even if there are not enough elements, just return the answer.
         * as well as operator-
         */
        /**
         * TODO iter++
         */
        iterator operator++(int) {
            iterator tmp=*this;
            ++*this;
            return tmp;
        }
        /**
         * TODO ++iter
         */
        iterator & operator++() {
            if (a==nullptr || x==nullptr || x==a->End) throw invalid_iterator();
            if (x->r){
                x=x->r;
                while (x->l) x=x->l;
            }else{
                while (x->fa){
                    if (x->fa->l==x) break;
                    x=x->fa;
                }
                if (x->fa) x=x->fa;else x=a->End;
            }
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator--(int) {
            iterator tmp=*this;
            --*this;
            return tmp;
        }
        /**
         * TODO --iter
         */
        iterator & operator--() {
            if (a==nullptr || x==nullptr) throw invalid_iterator();
            if (x==a->End){
                if (a->root){
                    x=a->root;while (x->r) x=x->r;
                    return *this;
                }else{
                    throw invalid_iterator();
                }
            }
            if (x->l){
                x=x->l;
                while (x->r) x=x->r;
            }else{
                while (x->fa){
                    if (x->fa->r==x) break;
                    x=x->fa;
                }
                if (x->fa) x=x->fa;else x=a->End;
            }
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        value_type & operator*() const {
            if (a==nullptr || x==nullptr) throw invalid_iterator();
            return x->value;
        }
        bool operator==(const iterator &rhs) const {return x==rhs.x;}
        bool operator==(const const_iterator &rhs) const {return x==rhs.it.x;}
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {return !(x==rhs.x);}
        bool operator!=(const const_iterator &rhs) const {return !(x==rhs.it.x);}

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        value_type* operator->() const noexcept {
            //if (a==nullptr || x==nullptr) throw invalid_iterator();
            return &(x->value);
        }
    };
    class const_iterator {
        // it should has similar member method as iterator.
        //  and it should be able to construct from an iterator.
        friend class map;
        friend class iterator;
    private:
        // data members.
        iterator it;
    public:
        const_iterator() {}
        const_iterator(const map *aa,node *xx){it.a=const_cast<map *>(aa);it.x=xx;}
        const_iterator(const const_iterator &other):it(other.it){}
        const_iterator(const iterator &other):it(other){}
        const_iterator operator++(int) {return const_iterator(it++);}
        const_iterator& operator++() {++it;return *this;}
        const_iterator operator--(int) {return const_iterator(it--);}
        const_iterator& operator--() {--it;return *this;}
        value_type& operator*() const {return *it;}
        value_type* operator->() const noexcept {return &(*it);}
        bool operator==(const iterator &rhs) const {return it==rhs;}
        bool operator==(const const_iterator &rhs) const {return it==rhs.it;}
        bool operator!=(const iterator &rhs) const {return it!=rhs;}
        bool operator!=(const const_iterator &rhs) const {return it!=rhs.it;}
    };
    /**
     * TODO two constructors
     */
    map(){root=nullptr;char *tmp=new char;End=(node*)tmp;sz=0;}
    map(const map &other){
        root=nullptr;char *tmp=new char;End=(node*)tmp;sz=0;
        for (const_iterator it=other.cbegin();it!=other.cend();it++) insert(*it);
    }
    /**
     * TODO assignment operator
     */
    map & operator=(const map &other) {
        if (this==&other) return *this;
        clear();
        for (const_iterator it=other.cbegin();it!=other.cend();it++) insert(*it);
        return *this;
    }
    /**
     * TODO Destructors
     */
    ~map() {
        del(root);
        delete (char*)End;
    }
    /**
     * TODO
     * access specified element with bounds checking
     * Returns a reference to the mapped value of the element with key equivalent to key.
     * If no such element exists, an exception of type `index_out_of_bound'
     */
    T & at(const Key &key) {
        iterator t=find(key);
        if (t==end()) throw index_out_of_bound();
        return t->second;
    }
    const T & at(const Key &key) const {
        const_iterator t=find(key);
        if (t==cend()) throw index_out_of_bound();
        return t->second;
    }
    /**
     * TODO
     * access specified element
     * Returns a reference to the value that is mapped to a key equivalent to key,
     *   performing an insertion if such key does not already exist.
     */
    T & operator[](const Key &key) {
        iterator t=find(key);
        if (t==end()){
            t=insert(value_type(key,T())).first;
        }
        return t->second;
    }
    /**
     * behave like at() throw index_out_of_bound if such key does not exist.
     */
    const T & operator[](const Key &key) const {return at(key);}
    /**
     * return a iterator to the beginning
     */
    iterator begin() {
        if (root){
            node *t=root;while (t->l) t=t->l;
            return iterator(this,t);
        }else{
            return end();
        }
    }
    const_iterator cbegin() const {
        if (root){
            node *t=root;while (t->l) t=t->l;
            return const_iterator(this,t);
        }else{
            return cend();
        }
    }
    /**
     * return a iterator to the end
     * in fact, it returns past-the-end.
     */
    iterator end() {return iterator(this,End);}
    const_iterator cend() const {return const_iterator(this,End);}
    /**
     * checks whether the container is empty
     * return true if empty, otherwise false.
     */
    bool empty() const {return sz==0;}
    /**
     * returns the number of elements.
     */
    size_t size() const {return sz;}
    /**
     * clears the contents
     */
    void clear() {
        del(root);
        root=nullptr;sz=0;
    }
    /**
     * insert an element.
     * return a pair, the first of the pair is
     *   the iterator to the new element (or the element that prevented the insertion),
     *   the second one is true if insert successfully, or false.
     */
    void insert_case(node *x){
        if (x->fa==nullptr){
            root=x;
            x->color=BLACK;
            return;
        }
        if (x->fa->color==RED){
            if (x->uncle() && x->uncle()->color==RED){
                x->fa->color=x->uncle()->color=BLACK;
                x->gfa()->color=RED;
                insert_case(x->gfa());
            }else{
                if (x==x->fa->r && x->fa==x->gfa()->l){
                    left_rotate(x->fa);
                    x=x->l;
                }else if (x==x->fa->l && x->fa==x->gfa()->r){
                    right_rotate(x->fa);
                    x=x->r;
                }
                x->fa->color=BLACK;x->gfa()->color=RED;
                if (x==x->fa->l) right_rotate(x->gfa());else left_rotate(x->gfa());
            }
        }
    }
    pair<iterator, bool> insert(const value_type &value) {
        if (root==nullptr){
            root=new node(value);
            root->color=BLACK;
            sz++;
            return pair<iterator, bool>(iterator(this,root),true);
        }
        node *x=root,*p=nullptr;
        while (x!=nullptr){
            p=x;
            if (Compare()(value.first,x->value.first)) x=x->l;
            else if (Compare()(x->value.first,value.first)) x=x->r;
            else return pair<iterator, bool>(iterator(this,x),false);
        }
        x=new node(value);x->fa=p;
        if (Compare()(value.first,p->value.first)) p->l=x;
        else if (Compare()(p->value.first,value.first)) p->r=x;
        insert_case(x);
        sz++;
        return pair<iterator, bool>(iterator(this,x),true);
    }
    /**
     * erase the element at pos.
     *
     * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
     */
    void erase_case(node *x){
        if (x->fa==nullptr){
            root=x;
            x->color=BLACK;
            return;
        }
        if (x->brother()->color==RED){
            x->brother()->color=BLACK;x->fa->color=RED;
            if (x==x->fa->l) left_rotate(x->fa);else right_rotate(x->fa);
        }
        if (x->fa->color==BLACK && x->brother()->color==BLACK && (x->brother()->l==nullptr || x->brother()->l->color==BLACK) && (x->brother()->r==nullptr || x->brother()->r->color==BLACK)){
            x->brother()->color=RED;
            erase_case(x->fa);
        }else{
            if (x->fa->color==RED && x->brother()->color==BLACK && (x->brother()->l==nullptr || x->brother()->l->color==BLACK) && (x->brother()->r==nullptr || x->brother()->r->color==BLACK)){
                x->brother()->color=RED;
                x->fa->color=BLACK;
            }else{
                if (x->brother()->color==BLACK){
                    if (x==x->fa->l && (x->brother()->l && x->brother()->l->color==RED) && (x->brother()->r==nullptr || x->brother()->r->color==BLACK)){
                        x->brother()->color=RED;x->brother()->l->color=BLACK;
                        right_rotate(x->brother());
                    }else if (x==x->fa->r && (x->brother()->r && x->brother()->r->color==RED) && (x->brother()->l==nullptr || x->brother()->l->color==BLACK)){
                        x->brother()->color=RED;x->brother()->r->color=BLACK;
                        left_rotate(x->brother());
                    }
                }
                x->brother()->color=x->fa->color;
                x->fa->color=BLACK;
                if (x==x->fa->l){
                    x->brother()->r->color=BLACK;
                    left_rotate(x->fa);
                }else{
                    x->brother()->l->color=BLACK;
                    right_rotate(x->fa);
                }
            }
        }
    }
    void erase(node *x){
        node *son=x->l?x->l:x->r;
        if (x->fa==nullptr){
            delete x;
            root=son;
            if (root) {root->fa=nullptr;root->color=BLACK;}
            return;
        }
        bool b=false;
        if (son==nullptr){
            son=new node(x->value);son->color=BLACK;
            b=true;
        }
        if (x->fa->l==x) x->fa->l=son;else x->fa->r=son;
        son->fa=x->fa;
        if (x->color==BLACK){
            if (son->color==RED){
                son->color=BLACK;
            }else{
                erase_case(son);
            }
        }
        delete x;
        if (b) {
            if (son==son->fa->l) son->fa->l=nullptr;else son->fa->r=nullptr;
            delete son;
        }
    }
    void erase(iterator pos) {
        if (pos.a!=this || pos.x==nullptr || pos.x==End) throw invalid_iterator();
        sz--;
        node *x=pos.x;
        if (x->r==nullptr){
            erase(x);
        }else{
            node *t=x->r;
            while (t->l) t=t->l;
            //swap(x,t);
            if (t->fa && t->fa==x){
                if (x->fa){if (x==x->fa->l) x->fa->l=t;else x->fa->r=t;}else root=t;
                std::swap(x->color,t->color);std::swap(x->fa,t->fa);std::swap(x->l,t->l);std::swap(x->r,t->r);
                t->r=x;x->fa=t;
                if (x->l) x->l->fa=x;if (x->r) x->r->fa=x;
                if (t->l) t->l->fa=t;if (t->r) t->r->fa=t;
            }else{
                if (x->fa){if (x==x->fa->l) x->fa->l=t;else x->fa->r=t;}else root=t;
                if (t->fa){if (t==t->fa->l) t->fa->l=x;else t->fa->r=x;}else root=x;
                std::swap(x->color,t->color);std::swap(x->fa,t->fa);std::swap(x->l,t->l);std::swap(x->r,t->r);
                if (x->l) x->l->fa=x;if (x->r) x->r->fa=x;
                if (t->l) t->l->fa=t;if (t->r) t->r->fa=t;
            }
            erase(x);
        }
    }
    /**
     * Returns the number of elements with key
     *   that compares equivalent to the specified argument,
     *   which is either 1 or 0
     *     since this container does not allow duplicates.
     * The default method of check the equivalence is !(a < b || b > a)
     */
    size_t count(const Key &key) const {return find(key)!=cend()?1:0;}
    /**
     * Finds an element with key equivalent to key.
     * key value of the element to search for.
     * Iterator to an element with key equivalent to key.
     *   If no such element is found, past-the-end (see end()) iterator is returned.
     */
    iterator find(const Key &key) {
        node *t=root;
        while (t!=nullptr){
            if (Compare()(key,t->value.first)) t=t->l;
            else if (Compare()(t->value.first,key)) t=t->r;
            else break;
        }
        if (t) return iterator(this,t);else return end();
    }
    const_iterator find(const Key &key) const {
        node *t=root;
        while (t!=nullptr){
            if (Compare()(key,t->value.first)) t=t->l;
            else if (Compare()(t->value.first,key)) t=t->r;
            else break;
        }
        if (t) return const_iterator(this,t);else return cend();
    }
};

}

#endif

