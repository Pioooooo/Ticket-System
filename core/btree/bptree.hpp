//
// Created by sunshine on 2020/5/13.
//

#ifndef BTREE_BPTREE_HPP
#define BTREE_BPTREE_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include "utility.hpp"

namespace sjtu {
    template <class Key, class Value, class Compare = std::less<Key>>
    class BTree {
    private:
        typedef pair<Key, Value> value_type;


    private:
        FILE *file;
        Compare cmp;
        static const size_t MAX_SIZ = 5;
        static const size_t MIN_SIZ = 2;

        class basic_info{
        public:
            char name[23];

            off_t root;
            off_t head;
            off_t tail;
            size_t siz;

            //free from merge
            size_t free_num;
            off_t free_pos[100];

            basic_info() {
                root = head = tail = siz = 0;
                free_num = 0;
                memset(name, 0, sizeof(name));
                memset(free_pos, 0, sizeof(free_pos));
            }

            ~basic_info()= default;
        };
//        static const size_t basic_siz = sizeof(basic_info);
        static const size_t basic_siz = 4096;
        basic_info *basic = nullptr;

        class Node{
        public:
            pair<Node *, size_t> par;
            off_t pos;
            bool is_leaf;
            size_t siz;
            off_t son[MAX_SIZ + 1];
            value_type val[MAX_SIZ + 1];

            off_t pre;
            off_t nex;

            Node(): pos(0), is_leaf(false), siz(0), pre(0), nex(0) {
                par.first = nullptr;
                par.second = 0;
                memset(val, 0, sizeof(val));
                memset(son, 0, sizeof(son));
            }
            explicit Node(off_t p): pos(p), is_leaf(true), siz(0), pre(0), nex(0) {
                par.first = nullptr;
                par.second = 0;
                memset(val, 0, sizeof(val));
                memset(son, 0, sizeof(son));
            }

            ~Node()= default;
        };
        typedef pair<Node *, size_t> position;
//        static const size_t Node_siz = sizeof(Node);
        static const size_t Node_siz = 4096;
        Node *root = nullptr;
        Node *head = nullptr;
        Node *tail = nullptr;

        static const size_t pool_siz = 997;
        Node *pool[pool_siz];
        size_t num = 0;
        size_t occupied[pool_siz];

        void _write( Node **p){
            if (*p != nullptr) {
                fseek(file, (*p)->pos, SEEK_SET);
                fwrite(*p, Node_siz, 1, file);
                //delete *p;
                free(*p);
                *p = nullptr;
            }
        }

        Node **_read(const off_t pos){ //without dealing with hash collision..........
            size_t p = pos % pool_siz;
            if (pool[p] == nullptr) {
                occupied[++num] = p;
                pool[p] = (Node *)malloc(Node_siz);
                memset(pool[p], 0, Node_siz);
                fseek(file, pos, SEEK_SET);
                fread(pool[p], Node_siz, 1, file);
                return &(pool[p]);
            } else if (pool[p]->pos == pos){
                return &(pool[p]);
            } else {
                fseek(file, pool[p]->pos, SEEK_SET);
                fwrite(pool[p], Node_siz, 1, file);
                fseek(file, pos, SEEK_SET);
                fread(pool[p], Node_siz, 1, file);
            }
        }

        void write_all(){
            while (num > 0){
                _write(&pool[occupied[num--]]);
            }
        }

        position search(const Key &key){
            Node *cur_node = root;
            position new_pos;
            while (!cur_node->is_leaf){
                size_t cur_siz = cur_node->siz;
                if (!cmp(key, cur_node->val[cur_siz].first)){
                    Node *tmp = *_read(cur_node->son[cur_siz]);
                    tmp->par.first = cur_node;
                    tmp->par.second = cur_siz;
                    cur_node = tmp;
                    continue;
                } else {
                    for (size_t i = 1; i <= cur_siz; i++){
                        if(cmp(key, cur_node->val[i].first)){
                            Node *tmp = *_read(cur_node->son[i - 1]);
                            tmp->par.first = cur_node;
                            tmp->par.second = i - 1;
                            cur_node = tmp;
                            break;
                        }
                    }
                }
            }
            for (size_t i = 1; i <= cur_node->siz; ++i){
                if(!cmp(cur_node->val[i].first, key) && !cmp(key, cur_node->val[i].first)){
                    new_pos.first = cur_node;
                    new_pos.second = i;
                    return new_pos;
                }
            }
            new_pos.first = nullptr;
            new_pos.second = 0;
            return new_pos;
        }

        position search_insert (const Key &key){
            Node *cur_node = root;
            position new_pos;
            while (!cur_node->is_leaf){
                int cur_siz = cur_node->siz;
                if (!cmp(key, cur_node->val[cur_siz].first)){
                    const off_t son_pos = cur_node->son[cur_siz];
                    Node *tmp = *_read(son_pos);
                    tmp->par.first = cur_node;
                    tmp->par.second = cur_siz;
                    cur_node = tmp;
                    continue;
                } else {
                    for (int i = 1; i <= cur_siz; i++){
                        if(cmp(key, cur_node->val[i].first)){
                            Node *tmp = *_read(cur_node->son[i - 1]);
                            tmp->par.first = cur_node;
                            tmp->par.second = i - 1;
                            cur_node = tmp;
                            break;
                        }
                    }
                }
            }
            size_t cur_siz = cur_node->siz;
            for (size_t i = 2; i <= cur_siz; ++i){
                if(!cmp(cur_node->val[i - 1].first, key)){
                    new_pos.first = nullptr;
                    new_pos.second = 0;
                    return new_pos;
                }
                if(cmp(key, cur_node->val[i].first)){
                    new_pos.first = cur_node;
                    new_pos.second = i - 1;
                    return new_pos;
                }
            }
            if(!cmp(cur_node->val[cur_siz].first, key)){
                new_pos.first = nullptr;
                new_pos.second = 0;
                return new_pos;
            }
            new_pos.first = cur_node;
            new_pos.second = cur_siz;
            return new_pos;
        }

        void split_leaf(Node *pos){
            off_t new_pos;
            if(basic->free_num == 0){
                fseek(file, 0, SEEK_END);
                new_pos = ftell(file);
            } else {
                new_pos = basic->free_pos[basic->free_num--];
            }
            size_t p = new_pos % pool_siz;
            occupied[++num] = p;
            pool[p] = (Node *)malloc(Node_siz);
            memset(pool[p], 0, Node_siz);
            Node *new_node = pool[p];
            new_node->pos = new_pos;
            new_node->is_leaf = true;
            new_node->siz = pos->siz - MIN_SIZ;
            new_node->pre = pos->pos;
            new_node->nex = pos->nex;
            pos->nex = new_pos;
            size_t sz = new_node->siz;
            for (size_t i = 1; i <= sz; ++i){
                new_node->val[i] = pos->val[MIN_SIZ + i];
            }
            pos->siz = MIN_SIZ;
            _write(&pool[p]);
            if (pos != root){
                insert_inner(pos->par, new_pos, pos->val[MIN_SIZ + 1]);
            } else {
                off_t np;
                if(basic->free_num == 0){
                    fseek(file, 0, SEEK_END);
                    np = ftell(file);
                } else {
                    np = basic->free_pos[basic->free_num--];
                }
                occupied[++num] = pos->pos % pool_siz;
                pool[pos->pos % pool_siz] = root;
                root = (Node *)malloc(Node_siz);
                memset(root, 0, Node_siz);
                root->pos = np;
                root->is_leaf = false;
                root->siz = 1;
                root->son[0] = pos->pos;
                root->son[1] = new_pos;
                root->val[1] = pos->val[MIN_SIZ + 1];
                basic->root = np;
                fseek(file, np, SEEK_SET);
                fwrite(root, Node_siz, 1, file);
            }
        }

        void split_inner(Node *pos){
            off_t new_pos;
            if(basic->free_num == 0){
                fseek(file, 0, SEEK_END);
                new_pos = ftell(file);
            } else {
                new_pos = basic->free_pos[basic->free_num--];
            }
            size_t p = new_pos % pool_siz;
            occupied[++num] = p;
            pool[p] = (Node *)malloc(Node_siz);
            memset(pool[p], 0, Node_siz);
            Node *new_node = pool[p];
            new_node->pos = new_pos;
            new_node->is_leaf = false;
            new_node->siz = pos->siz - MIN_SIZ - 1;
            new_node->son[0] = pos->son[MIN_SIZ + 1];
            size_t sz = new_node->siz;
            for (size_t i = 1; i <= sz; ++i){
                new_node->val[i] = pos->val[MIN_SIZ + 1 + i];
                new_node->son[i] = pos->son[MIN_SIZ + 1 + i];
            }
            pos->siz = MIN_SIZ;
            _write(&pool[p]);
            if (pos != root) {
                insert_inner(pos->par, new_pos, pos->val[MIN_SIZ + 1]);
            } else {
                off_t np;
                if(basic->free_num == 0){
                    fseek(file, 0, SEEK_END);
                    np = ftell(file);
                } else {
                    np = basic->free_pos[basic->free_num--];
                }
                occupied[++num] = pos->pos % pool_siz;
                pool[pos->pos % pool_siz] = root;
                root = (Node *)malloc(Node_siz);
                memset(root, 0, Node_siz);
                root->pos = np;
                root->is_leaf = false;
                root->siz = 1;
                root->son[0] = pos->pos;
                root->son[1] = new_pos;
                root->val[1] = pos->val[MIN_SIZ + 1];
                basic->root = np;
                fseek(file, np, SEEK_SET);
                fwrite(root, Node_siz, 1, file);
            }
        }

        void insert_inner(position pre, off_t &s, value_type &v) {
            Node *cur_node = pre.first;
            size_t cur_pos = pre.second + 1;
            for (size_t i = ++cur_node->siz; i > cur_pos; i--){
                cur_node->son[i] = cur_node->son[i-1];
                cur_node->val[i] = cur_node->val[i-1];
            }
            cur_node->son[cur_pos] = s;
            cur_node->val[cur_pos] = v;
            if (cur_node->siz == MAX_SIZ)
                split_inner(cur_node);
        }

        void modify_inner(position &pos, value_type &v){
            if (pos.second == 0 && pos.first != root){
                modify_inner(pos.first->par, v);
                return;
            }
            pos.first->val[pos.second] = v;
        }

        void merge_leaf(Node *pos){
            Node *par = pos->par.first;
            size_t cur_pos = pos->par.second;
            if (cur_pos == 0){
                size_t bro_pos = par->son[1];
                Node **bro_ = _read(bro_pos);
                Node *bro_node = *bro_;
                if(bro_node->siz > MIN_SIZ){
                    pos->val[++pos->siz] = bro_node->val[1];
                    size_t bro_siz = --bro_node->siz;
                    for(size_t i = 1; i <= bro_siz; ++i){
                        bro_node->val[i] = bro_node->val[i+1];
                    }
                    position parent;
                    parent.first = par;
                    parent.second = 1;
                    modify_inner(parent, bro_node->val[1]);
                    return;
                } else {
                    Node **tmp = _read(bro_node->nex);
                    (*tmp)->pre = pos->pos;
                    pos->nex = (*tmp)->pos;
                    _write(tmp);
                    size_t ini = pos->siz;
                    size_t sz = bro_node->siz;
                    pos->siz += bro_node->siz;
                    for(size_t i = 1; i <= sz; ++i){
                        pos->val[ini+i] = bro_node->val[i];
                    }
                    basic->free_pos[++basic->free_num] = bro_pos;
//                    delete bro_node;
                    free(bro_node);
                    *bro_ = nullptr;
                    position parent;
                    parent.first = par;
                    parent.second = 0;
                    erase_inner(parent);
                    return;
                }
            } else{
                size_t bro_pos = par->son[cur_pos - 1];
                Node **bro_ = _read(bro_pos);
                Node *bro_node = *bro_;
                if(bro_node->siz > MIN_SIZ){
                    for(size_t i = ++pos->siz; i > 1; --i){
                        pos->val[i] = pos->val[i-1];
                    }
                    pos->val[1] = bro_node->val[bro_node->siz--];
                    modify_inner(pos->par, pos->val[1]);
                    return;
                } else {
                    Node **tmp = _read(bro_node->pre);
                    (*tmp)->nex = pos->pos;
                    pos->pre = (*tmp)->pos;
                    _write(tmp);
                    size_t ini = bro_node->siz;
                    size_t sz = pos->siz;
                    bro_node->siz += sz;
                    for(size_t i = 1; i <= sz; ++i){
                        bro_node->val[ini+i] = pos->val[i];
                    }
                    basic->free_pos[++basic->free_num] = pos->pos;
///                    delete pos;    // costs more memory
                    position parent;
                    parent.first = par;
                    parent.second = cur_pos;
                    erase_inner(parent);
                    return;
                }
            }
        }

        void merge_inner(Node *pos){
            Node *par = pos->par.first;
            size_t cur_pos = pos->par.second;
            if (cur_pos == 0){
                size_t bro_pos = par->son[1];
                Node **bro_ = _read(bro_pos);
                Node *bro_node = *bro_;
                if(bro_node->siz > MIN_SIZ){
                    pos->son[++pos->siz] = bro_node->son[0];
                    pos->val[pos->siz] = par->val[1];
                    bro_node->son[0] = bro_node->son[1];
                    size_t bro_siz = --bro_node->siz;
                    for(size_t i = 1; i <= bro_siz; ++i){
                        bro_node->son[i] = bro_node->son[i+1];
                        bro_node->val[i] = bro_node->val[i+1];
                    }
                    position parent;
                    parent.first = par;
                    parent.second = 1;
                    modify_inner(parent, bro_node->val[1]);
                    return;
                } else {
                    size_t ini = pos->siz + 1;
                    size_t sz = bro_node->siz;
                    pos->siz += bro_node->siz + 1;
                    pos->son[ini] = bro_node->son[0];
                    pos->val[ini] = par->val[1];
                    for(size_t i = 1; i <= sz; ++i){
                        pos->val[ini+i] = bro_node->val[i];
                    }
                    basic->free_pos[++basic->free_num] = bro_pos;
                    free(bro_node);
                    *bro_ = nullptr;
//                    delete bro_node;
                    position parent;
                    parent.first = par;
                    parent.second = 0;
                    erase_inner(parent);
                    return;
                }
            } else{
                size_t bro_pos = par->son[cur_pos - 1];
                Node *bro_node = *_read(bro_pos);
                if(bro_node->siz > MIN_SIZ){
                    for(size_t i = ++pos->siz; i > 1; --i){
                        pos->son[i] = pos->son[i-1];
                        pos->val[i] = pos->val[i-1];
                    }
                    pos->son[0] = bro_node->son[bro_node->siz];
                    pos->val[1] = par->val[cur_pos];
                    modify_inner(pos->par, bro_node->val[bro_node->siz--]);
                    return;
                } else {
                    size_t ini = bro_node->siz + 1;
                    size_t sz = pos->siz;
                    bro_node->siz += sz;
                    bro_node->son[ini] = pos->son[0];
                    bro_node->val[ini] = par->val[cur_pos];
                    for(size_t i = 1; i <= sz; ++i){
                        bro_node->son[ini+i] = pos->son[i];
                        bro_node->val[ini+i] = pos->val[i];
                    }
                    basic->free_pos[++basic->free_num] = pos->pos;
///                    free(pos);
//                    delete pos;
                    position parent;
                    parent.first = par;
                    parent.second = cur_pos;
                    erase_inner(parent);
                    return;
                }
            }
        }

        void erase_inner(position &pos){
            Node *cur_node = pos.first;
            size_t cur_pos = pos.second;
            size_t sz = --cur_node->siz;
            if(cur_pos != 0){
                for(size_t i = cur_pos; i <= sz; ++i){
                    cur_node->son[i] = cur_node->son[i+1];
                    cur_node->val[i] = cur_node->val[i+1];
                }
            } else {
                cur_node->son[0] = cur_node->son[1];
                for(size_t i = cur_pos; i <= sz; ++i){
                    cur_node->son[i] = cur_node->son[i+1];
                    cur_node->val[i] = cur_node->val[i+1];
                }
            }
            if(cur_node == root){
                if(sz == 0){
                    Node *tmp = (Node *)malloc(Node_siz);
                    memset(tmp, 0, Node_siz);
                    fseek(file, root->son[0], SEEK_SET);
                    fread(tmp, Node_siz, 1, file);
                    basic->root = tmp->pos;
                    basic->free_pos[++basic->free_num] = root->pos;
                    free(root);
//                    delete root;
                    root = tmp;
                }
                return;
            }
            if(sz < MIN_SIZ){
                merge_inner(cur_node);
            }
        }

    public:

        explicit BTree(const char *fname = "data") {
            file = fopen(fname, "rb+");
            basic = (basic_info *)malloc(basic_siz);
//            basic = new basic_info();
            if (file){
                fseek(file, 0, SEEK_SET);
                fread(basic, basic_siz, 1, file);
                root = (Node *)malloc(Node_siz);
                fseek(file, basic->root, SEEK_SET);
                fread(root, Node_siz, 1, file);
            } else {
                memset(basic, 0, basic_siz);
                file = fopen(fname, "wb+");
                strcpy(basic->name, fname);
                fseek(file, 0, SEEK_SET);
                fwrite(basic, basic_siz, 1, file);
//                root = new Node(basic_siz);
                root = (Node *)malloc(Node_siz);
                memset(root, 0, Node_siz);
                fseek(file, 0, SEEK_END);
                root->pos = ftell(file);
                fwrite(root, Node_siz, 1, file);
                basic->root = root->pos;
//                head = new Node(root->pos + Node_siz);
                head = (Node *)malloc(Node_siz);
                memset(head, 0, Node_siz);
                fseek(file, 0, SEEK_END);
                head->pos = ftell(file);
                fwrite(head, Node_siz, 1, file);
                basic->head = head->pos;
//                tail = new Node(head->pos + Node_siz);
                tail = (Node *)malloc(Node_siz);
                memset(tail, 0, Node_siz);
                fseek(file, 0, SEEK_END);
                tail->pos = ftell(file);
                fwrite(tail, Node_siz, 1, file);
                basic->tail = tail->pos;
                root->is_leaf = true;
                head->nex = root->pos;
                root->pre = head->pos;
                tail->pre = root->pos;
                root->nex = tail->pos;
                basic->siz = 0;
                basic->free_num = 0;
                fseek(file, 0, SEEK_SET);
                fwrite(basic, basic_siz, 1, file);
                fseek(file, root->pos, SEEK_SET);
                fwrite(root, Node_siz, 1, file);
                _write(&head);
                _write(&tail);
            }

        }

        ~BTree() {
            for (size_t i = 0; i < pool_siz; ++i){
                if (pool[i] != nullptr)
                    _write(&(pool[i]));
            }
            if (basic != nullptr) {
                fseek(file, 0, SEEK_SET);
                fwrite(basic, Node_siz, 1, file);
            }
            _write(&root);
            _write(&head);
            _write(&tail);
            fclose(file);
        }

        // Clear the BTree
        void clear() {
            for (size_t i = 0; i < pool_siz; ++i){
                if (pool[i] != nullptr) {
                    free(pool[i]);
//                    delete pool[i];
                }
            }
            //delete root;
            free(root);

            fclose(file);
            file = fopen(basic->name, "wb+");
            fseek(file, 0, SEEK_SET);
            fwrite(basic, basic_siz, 1, file);
//                root = new Node(basic_siz);
            root = (Node *)malloc(Node_siz);
            memset(root, 0, Node_siz);
            fseek(file, 0, SEEK_END);
            root->pos = ftell(file);
            fwrite(root, Node_siz, 1, file);
            basic->root = root->pos;
//                head = new Node(root->pos + Node_siz);
            head = (Node *)malloc(Node_siz);
            memset(head, 0, Node_siz);
            fseek(file, 0, SEEK_END);
            head->pos = ftell(file);
            fwrite(head, Node_siz, 1, file);
            basic->head = head->pos;
//                tail = new Node(head->pos + Node_siz);
            tail = (Node *)malloc(Node_siz);
            memset(tail, 0, Node_siz);
            fseek(file, 0, SEEK_END);
            tail->pos = ftell(file);
            fwrite(tail, Node_siz, 1, file);
            basic->tail = tail->pos;
            root->is_leaf = true;
            head->nex = root->pos;
            root->pre = head->pos;
            tail->pre = root->pos;
            root->nex = tail->pos;
            basic->siz = 0;
            basic->free_num = 0;
            fseek(file, 0, SEEK_SET);
            fwrite(basic, basic_siz, 1, file);
            fseek(file, root->pos, SEEK_SET);
            fwrite(root, Node_siz, 1, file);
            _write(&head);
            _write(&tail);
        }

        size_t size() {
            return basic->siz;
        }

        bool insert(const Key &key, const Value &value) {
            if (basic->siz == 0){
                root->is_leaf = true;
                root->siz++;
                root->val[1].first = key;
                root->val[1].second = value;
                basic->siz++;
                return true;
            }
            position cur = search_insert(key);
            if (cur.first == nullptr){
                write_all();
                return false;
            }
            basic->siz++;
            Node *cur_node = cur.first;
            size_t cur_pos = cur.second + 1;
            for (size_t i = ++cur_node->siz; i > cur_pos; i--){
                cur_node->val[i] = cur_node->val[i-1];
            }
            cur_node->val[cur_pos].first = key;
            cur_node->val[cur_pos].second = value;
            if (cur_node->siz == MAX_SIZ)
                split_leaf(cur_node);
            write_all();
            return true;
        }

        bool modify(const Key &key, const Value &value) {
            position p = search(key);
            if (p.first != nullptr){
                p.first->val[p.second].second = value;
                write_all();
                return true;
            }
            return false;
        }

        Value at(const Key &key) {
            position p = search(key);
            if (p.first != nullptr){
                Value val = p.first->val[p.second].second;
                write_all();
                return val;
            }
            return Value();
        }

        bool find(const Key &key) {
            position p = search(key);
            if (p.first != nullptr)
                return true;
            return false;
        }

        bool end(){
            return false;
        }

        bool erase(const Key &key) {
            if (basic->siz == 0) return false;
            position pos = search(key);
            if (pos.first == nullptr)
                return false;
            --basic->siz;
            Node *cur_node = pos.first;
            size_t cur_pos = pos.second;
            size_t sz = --cur_node->siz;
            for(size_t i = cur_pos; i <= sz; ++i){
                cur_node->val[i] = cur_node->val[i+1];
            }
            if(cur_node == root){
                return true;
            }
            if(cur_pos == 1) {
                modify_inner(cur_node->par, cur_node->val[1]);
            }
            if(sz < MIN_SIZ){
                merge_leaf(cur_node);
            }

            write_all();
            return true;
        }
    };
}


#endif //BTREE_BPTREE_HPP
