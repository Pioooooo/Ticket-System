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
        typedef pair<Key, off_t> sub_node;
        ///Key & son_address or Key & info_address in another file


    private:
        FILE *file;
        FILE *val_file;
        Compare cmp;
        static const size_t MAX_SIZ = 200;
        static const size_t MIN_SIZ = 100;

        class basic_info{
        public:
            char name[25];
            char name2[25];

            off_t root;
//            off_t head;
//            off_t tail;
            size_t siz;

            //free from merge
            size_t free_num;
            off_t free_pos[200];

            size_t free_num2;
            off_t free_pos2[200];

            basic_info() {
                root = siz = 0;
                free_num = 0;
                free_num2 = 0;
                memset(name, 0, sizeof(name));
                memset(free_pos, 0, sizeof(free_pos));
                memset(free_pos2, 0, sizeof(free_pos2));
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
            sub_node info[MAX_SIZ + 1];

//            off_t pre;
//            off_t nex;

            Node(): pos(0), is_leaf(false), siz(0){
                par.first = nullptr;
                par.second = 0;
                memset(info, 0, sizeof(info));
            }
            explicit Node(off_t p): pos(p), is_leaf(true), siz(0){
                par.first = nullptr;
                par.second = 0;
                memset(info, 0, sizeof(info));
            }

            ~Node()= default;
        };
        typedef pair<Node *, size_t> position;
//        static const size_t Node_siz = sizeof(Node);
        static const size_t Node_siz = 4096;
        Node *root = nullptr;
//        Node *head = nullptr;
//        Node *tail = nullptr;

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
                return &(pool[p]);
            }
        }

        void write_all(){
            if (num < 350)
                return;
            while (num > 0){
                _write(&pool[occupied[num--]]);
            }
        }

        position search(const Key &key){
            Node *cur_node = root;
            position new_pos;
            while (!cur_node->is_leaf){
                size_t cur_siz = cur_node->siz;
                if (!cmp(key, cur_node->info[cur_siz].first)){
                    Node *tmp = *_read(cur_node->info[cur_siz].second);
                    tmp->par.first = cur_node;
                    tmp->par.second = cur_siz;
                    cur_node = tmp;
                    continue;
                } else {
                    for (size_t i = 1; i <= cur_siz; i++){
                        if(cmp(key, cur_node->info[i].first)){
                            Node *tmp = *_read(cur_node->info[i - 1].second);
                            tmp->par.first = cur_node;
                            tmp->par.second = i - 1;
                            cur_node = tmp;
                            break;
                        }
                    }
                }
            }
            for (size_t i = 1; i <= cur_node->siz; ++i){
//                if(!cmp(cur_node->info[i].first, key) && !cmp(key, cur_node->info[i].first)){
//                    new_pos.first = cur_node;
//                    new_pos.second = i;
//                    return new_pos;
//                }
                if(cmp(cur_node->info[i].first, key))
                    continue;
                if(cmp(key, cur_node->info[i].first))
                    break;
                new_pos.first = cur_node;
                new_pos.second = i;
                return new_pos;
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
                if (!cmp(key, cur_node->info[cur_siz].first)){
                    const off_t son_pos = cur_node->info[cur_siz].second;
                    Node *tmp = *_read(son_pos);
                    tmp->par.first = cur_node;
                    tmp->par.second = cur_siz;
                    cur_node = tmp;
                    continue;
                } else {
                    for (int i = 1; i <= cur_siz; i++){
                        if(cmp(key, cur_node->info[i].first)){
                            Node *tmp = *_read(cur_node->info[i - 1].second);
                            tmp->par.first = cur_node;
                            tmp->par.second = i - 1;
                            cur_node = tmp;
                            break;
                        }
                    }
                }
            }
            size_t cur_siz = cur_node->siz;
            if(cmp(key, cur_node->info[1].first)){
                new_pos.first = cur_node;
                new_pos.second = 0;
                return new_pos;
            }
            for (size_t i = 2; i <= cur_siz; ++i){
                if(!cmp(cur_node->info[i - 1].first, key)){
                    new_pos.first = nullptr;
                    new_pos.second = 0;
                    return new_pos;
                }
                if(cmp(key, cur_node->info[i].first)){
                    new_pos.first = cur_node;
                    new_pos.second = i - 1;
                    return new_pos;
                }
            }
            if(!cmp(cur_node->info[cur_siz].first, key)){
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
            if(pool[p] == nullptr) {
                occupied[++num] = p;
            } else{
                fseek(file, pool[p]->pos, SEEK_SET);
                fwrite(pool[p], Node_siz, 1, file);
            }
            pool[p] = (Node *)malloc(Node_siz);
            memset(pool[p], 0, Node_siz);
            Node *new_node = pool[p];
            new_node->pos = new_pos;
            new_node->is_leaf = true;
            new_node->siz = pos->siz - MIN_SIZ;
//            new_node->pre = pos->pos;
//            new_node->nex = pos->nex;
//            pos->nex = new_pos;
            size_t sz = new_node->siz;
            for (size_t i = 1; i <= sz; ++i){
                new_node->info[i] = pos->info[MIN_SIZ + i];
            }
            pos->siz = MIN_SIZ;
            _write(&pool[p]);
            if (pos != root){
                insert_inner(pos->par, new_pos, pos->info[MIN_SIZ + 1].first);
            } else {
                off_t np;
                if(basic->free_num == 0){
                    fseek(file, 0, SEEK_END);
                    np = ftell(file);
                } else {
                    np = basic->free_pos[basic->free_num--];
                }
                size_t _p = pos->pos % pool_siz;
                if(pool[_p] == nullptr) {
                    occupied[++num] = _p;
                    pool[_p] = root;
                } else{
                    fseek(file, pool[_p]->pos, SEEK_SET);
                    fwrite(pool[_p], Node_siz, 1, file);
                    pool[_p] = root;
                }
                root = (Node *)malloc(Node_siz);
                memset(root, 0, Node_siz);
                root->pos = np;
                root->is_leaf = false;
                root->siz = 1;
                root->info[0].second = pos->pos;
                root->info[1].second = new_pos;
                root->info[1].first = pos->info[MIN_SIZ + 1].first;
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
            if(pool[p] == nullptr) {
                occupied[++num] = p;
            } else{
                fseek(file, pool[p]->pos, SEEK_SET);
                fwrite(pool[p], Node_siz, 1, file);
            }
            pool[p] = (Node *)malloc(Node_siz);
            memset(pool[p], 0, Node_siz);
            Node *new_node = pool[p];
            new_node->pos = new_pos;
            new_node->is_leaf = false;
            new_node->siz = pos->siz - MIN_SIZ - 1;
            new_node->info[0].second = pos->info[MIN_SIZ + 1].second;
            size_t sz = new_node->siz;
            for (size_t i = 1; i <= sz; ++i){
                new_node->info[i] = pos->info[MIN_SIZ + 1 + i];
            }
            pos->siz = MIN_SIZ;
            _write(&pool[p]);
            if (pos != root) {
                insert_inner(pos->par, new_pos, pos->info[MIN_SIZ + 1].first);
            } else {
                off_t np;
                if(basic->free_num == 0){
                    fseek(file, 0, SEEK_END);
                    np = ftell(file);
                } else {
                    np = basic->free_pos[basic->free_num--];
                }
                size_t _p = pos->pos % pool_siz;
                if(pool[_p] == nullptr) {
                    occupied[++num] = _p;
                } else{
                    fseek(file, pool[_p]->pos, SEEK_SET);
                    fwrite(pool[_p], Node_siz, 1, file);
                }
                pool[_p] = root;
                root = (Node *)malloc(Node_siz);
                memset(root, 0, Node_siz);
                root->pos = np;
                root->is_leaf = false;
                root->siz = 1;
                root->info[0].second = pos->pos;
                root->info[1].second = new_pos;
                root->info[1].first = pos->info[MIN_SIZ + 1].first;
                basic->root = np;
                fseek(file, np, SEEK_SET);
                fwrite(root, Node_siz, 1, file);
            }
        }

        void insert_inner(position pre, off_t &s, Key &key) {
            Node *cur_node = pre.first;
            size_t cur_pos = pre.second + 1;
            for (size_t i = ++cur_node->siz; i > cur_pos; i--){
                cur_node->info[i] = cur_node->info[i - 1];
            }
            cur_node->info[cur_pos].second = s;
            cur_node->info[cur_pos].first = key;
            if (cur_node->siz == MAX_SIZ)
                split_inner(cur_node);
        }

        void modify_inner(position &pos, Key &key){
            if (pos.second == 0 && pos.first != root){
                modify_inner(pos.first->par, key);
                return;
            }
            pos.first->info[pos.second].first = key;
        }

        void merge_leaf(Node *pos){
            Node *par = pos->par.first;
            size_t cur_pos = pos->par.second;
            if (cur_pos == 0){
                off_t bro_pos = par->info[1].second;
                Node **bro_ = _read(bro_pos);
                Node *bro_node = *bro_;
                if(bro_node->siz > MIN_SIZ){
                    pos->info[++pos->siz] = bro_node->info[1];
                    size_t bro_siz = --bro_node->siz;
                    for(size_t i = 1; i <= bro_siz; ++i){
                        bro_node->info[i] = bro_node->info[i + 1];
                    }
                    position parent;
                    parent.first = par;
                    parent.second = 1;
                    modify_inner(parent, bro_node->info[1].first);
                    return;
                } else {
//                    Node **tmp = _read(bro_node->nex);
//                    (*tmp)->pre = pos->pos;
//                    pos->nex = (*tmp)->pos;
//                    _write(tmp);
                    size_t ini = pos->siz;
                    size_t sz = bro_node->siz;
                    pos->siz += sz;
                    for(size_t i = 1; i <= sz; ++i){
                        pos->info[ini + i] = bro_node->info[i];
                    }
                    basic->free_pos[++basic->free_num] = bro_pos;
//                    delete bro_node;
                    free(bro_node);
                    *bro_ = nullptr;
                    position parent;
                    parent.first = par;
                    parent.second = 1;
                    if (par != root || root->siz != 1) {
                        erase_inner(parent);
                        return;
                    } else {
                        free(root);
                        root = pos;
                        basic->root = pos->pos;
                        pool[pos->pos % pool_siz] = nullptr;
                    }
                }
            } else{
                off_t bro_pos = par->info[cur_pos - 1].second;
                Node **bro_ = _read(bro_pos);
                Node *bro_node = *bro_;
                if(bro_node->siz > MIN_SIZ){
                    for(size_t i = ++pos->siz; i > 1; --i){
                        pos->info[i] = pos->info[i - 1];
                    }
                    pos->info[1] = bro_node->info[bro_node->siz--];
                    modify_inner(pos->par, pos->info[1].first);
                    return;
                } else {
//                    Node **tmp = _read(pos->nex);
//                    (*tmp)->pre = bro_pos;
//                    pos->pre = (*tmp)->pos;
//                    _write(tmp);
                    size_t ini = bro_node->siz;
                    size_t sz = pos->siz;
                    bro_node->siz += sz;
                    for(size_t i = 1; i <= sz; ++i){
                        bro_node->info[ini + i] = pos->info[i];
                    }
                    basic->free_pos[++basic->free_num] = pos->pos;
///                    delete pos;    // costs more memory
                    position parent;
                    parent.first = par;
                    parent.second = cur_pos;
                    if (par != root || root->siz != 1) {
                        erase_inner(parent);
                        return;
                    } else {
                        free(root);
                        root = bro_node;
                        basic->root = bro_pos;
                        pool[bro_pos % pool_siz] = nullptr;
                    }
                }
            }
        }

        void merge_inner(Node *pos){
            Node *par = pos->par.first;
            size_t cur_pos = pos->par.second;
            if (cur_pos == 0){
                off_t bro_pos = par->info[1].second;
                Node **bro_ = _read(bro_pos);
                Node *bro_node = *bro_;
                if(bro_node->siz > MIN_SIZ){
                    pos->info[++pos->siz].second = bro_node->info[0].second;
                    pos->info[pos->siz].first = par->info[1].first;
                    bro_node->info[0].second = bro_node->info[1].second;
                    size_t bro_siz = --bro_node->siz;
                    for(size_t i = 1; i <= bro_siz; ++i){
                        bro_node->info[i] = bro_node->info[i + 1];
                    }
                    position parent;
                    parent.first = par;
                    parent.second = 0;
                    modify_inner(parent, bro_node->info[1].first);
                    return;
                } else {
                    size_t ini = pos->siz + 1;
                    size_t sz = bro_node->siz;
                    pos->siz += bro_node->siz + 1;
                    pos->info[ini].second = bro_node->info[0].second;
                    pos->info[ini].first = par->info[1].first;
                    for(size_t i = 1; i <= sz; ++i){
                        pos->info[ini + i] = bro_node->info[i];
                    }
                    basic->free_pos[++basic->free_num] = bro_pos;
                    free(bro_node);
                    *bro_ = nullptr;
//                    delete bro_node;
                    position parent;
                    parent.first = par;
                    parent.second = 1;
                    if (par != root || root->siz != 1) {
                        erase_inner(parent);
                        return;
                    } else {
                        free(root);
                        root = pos;
                        basic->root = pos->pos;
                        pool[pos->pos % pool_siz] = nullptr;
                    }
                }
            } else{
                off_t bro_pos = par->info[cur_pos - 1].second;
                Node **bro_ = _read(bro_pos);
                Node *bro_node = *bro_;
                if(bro_node->siz > MIN_SIZ){
                    for(size_t i = ++pos->siz; i > 1; --i){
                        pos->info[i] = pos->info[i - 1];
                    }
                    pos->info[0].second = bro_node->info[bro_node->siz].second;
                    pos->info[1].first = par->info[cur_pos].first;
                    modify_inner(pos->par, bro_node->info[bro_node->siz--].first);
                    return;
                } else {
                    size_t ini = bro_node->siz + 1;
                    size_t sz = pos->siz;
                    bro_node->siz += sz;
                    bro_node->info[ini].second = pos->info[0].second;
                    bro_node->info[ini].first = par->info[cur_pos].first;
                    for(size_t i = 1; i <= sz; ++i){
                        bro_node->info[ini + i] = pos->info[i];
                    }
                    basic->free_pos[++basic->free_num] = pos->pos;
///                    free(pos);
//                    delete pos;
                    position parent;
                    parent.first = par;
                    parent.second = cur_pos;
                    if (par != root || root->siz != 1) {
                        erase_inner(parent);
                        return;
                    } else{
                        free(root);
                        root = bro_node;
                        basic->root = bro_pos;
                        *bro_ = nullptr;
                    }
                }
            }
        }

        void erase_inner(position &pos){
            Node *cur_node = pos.first;
            size_t cur_pos = pos.second;
            size_t sz = --cur_node->siz;
            if(cur_pos != 0){
                for(size_t i = cur_pos; i <= sz; ++i){
                    cur_node->info[i] = cur_node->info[i + 1];
                }
            } else {
                cur_node->info[0].second = cur_node->info[1].second;
                for(size_t i = cur_pos; i <= sz; ++i){
                    cur_node->info[i] = cur_node->info[i + 1];
                }
            }
//            if(cur_node == root){
//                if(sz == 0){
//                    write_all();
//                    Node *tmp = (Node *)malloc(Node_siz);
//                    memset(tmp, 0, Node_siz);
//                    fseek(file, root->info[0].first, SEEK_SET);
//                    fread(tmp, Node_siz, 1, file);
//                    basic->root = tmp->pos;
//                    basic->free_pos[++basic->free_num] = root->pos;
//                    free(root);
//                    delete root;
//                    root = tmp;
//                }
//                return;
//            }
            if (cur_node == root) {
                return;
            }
            if(sz < MIN_SIZ){
                merge_inner(cur_node);
            }
        }

    public:

        explicit BTree(const char *file_name1 = "data1", const char *file_name2 = "data2") {
//            fopen_s(&file, file_name1, "rb+");
//            fopen_s(&val_file, file_name2, "rb+");
            file = fopen(file_name1, "rb+");
            val_file = fopen(file_name2, "rb+");

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
//                fopen_s(&file, file_name1, "wb+");
//                fopen_s(&val_file, file_name2, "wb+");
                file = fopen(file_name1, "wb+");
                val_file = fopen(file_name2, "wb+");
//                strcpy_s(basic->name, file_name1);
//                strcpy_s(basic->name2, file_name2);
                strcpy(basic->name, file_name1);
                strcpy(basic->name2, file_name2);
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
//                head = (Node *)malloc(Node_siz);
//                memset(head, 0, Node_siz);
//                fseek(file, 0, SEEK_END);
//                head->pos = ftell(file);
//                fwrite(head, Node_siz, 1, file);
//                basic->head = head->pos;
//                tail = new Node(head->pos + Node_siz);
//                tail = (Node *)malloc(Node_siz);
//                memset(tail, 0, Node_siz);
//                fseek(file, 0, SEEK_END);
//                tail->pos = ftell(file);
//                fwrite(tail, Node_siz, 1, file);
//                basic->tail = tail->pos;
                root->is_leaf = true;
//                head->nex = root->pos;
//                root->pre = head->pos;
//                tail->pre = root->pos;
//                root->nex = tail->pos;
                basic->siz = 0;
                basic->free_num = 0;
                fseek(file, 0, SEEK_SET);
                fwrite(basic, basic_siz, 1, file);
                fseek(file, root->pos, SEEK_SET);
                fwrite(root, Node_siz, 1, file);
//                _write(&head);
//                _write(&tail);
            }

        }

        ~BTree() {
//            for (size_t i = 0; i < pool_siz; ++i){
//                if (pool[i] != nullptr)
//                    _write(&(pool[i]));
//            }
            while (num > 0){
                _write(&(pool[occupied[num--]]));
            }
            if (basic != nullptr) {
                fseek(file, 0, SEEK_SET);
                fwrite(basic, Node_siz, 1, file);
            }
            _write(&root);
//            _write(&head);
//            _write(&tail);
            fclose(file);
            fclose(val_file);
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
            fclose(val_file);
            val_file = fopen(basic->name2, "wb+");
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
//            head = (Node *)malloc(Node_siz);
//            memset(head, 0, Node_siz);
//            fseek(file, 0, SEEK_END);
//            head->pos = ftell(file);
//            fwrite(head, Node_siz, 1, file);
//            basic->head = head->pos;
//                tail = new Node(head->pos + Node_siz);
//            tail = (Node *)malloc(Node_siz);
//            memset(tail, 0, Node_siz);
//            fseek(file, 0, SEEK_END);
//            tail->pos = ftell(file);
//            fwrite(tail, Node_siz, 1, file);
//            basic->tail = tail->pos;
            root->is_leaf = true;
//            head->nex = root->pos;
//            root->pre = head->pos;
//            tail->pre = root->pos;
//            root->nex = tail->pos;
            basic->siz = 0;
            basic->free_num = 0;
            fseek(file, 0, SEEK_SET);
            fwrite(basic, basic_siz, 1, file);
            fseek(file, root->pos, SEEK_SET);
            fwrite(root, Node_siz, 1, file);
//            _write(&head);
//            _write(&tail);
        }

        size_t size() {
            return basic->siz;
        }

        bool insert(const Key &key, const Value &value) {
            if (basic->siz == 0){
                root->is_leaf = true;
                root->siz++;
                root->info[1].first = key;
                if(basic->free_num2 != 0){
                    fseek(val_file, basic->free_pos2[basic->free_num2--], SEEK_SET);
                } else{
                    fseek(val_file, 0, SEEK_END);
                }
                root->info[1].second = ftell(val_file);
                fwrite(&value, sizeof(Value), 1, val_file);
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
                cur_node->info[i] = cur_node->info[i - 1];
            }
            cur_node->info[cur_pos].first = key;
            if(basic->free_num2 != 0){
                fseek(val_file, basic->free_pos2[basic->free_num2--], SEEK_SET);
            } else{
                fseek(val_file, 0, SEEK_END);
            }
            cur_node->info[cur_pos].second = ftell(val_file);
            fwrite(&value, sizeof(value), 1, val_file);
            if (cur_node->siz == MAX_SIZ)
                split_leaf(cur_node);
            write_all();
            return true;
        }

        bool modify(const Key &key, const Value &value) {
            position p = search(key);
            if (p.first != nullptr){
                fseek(val_file, p.first->info[p.second].second, SEEK_SET);
                fwrite(&value, sizeof(value), 1, val_file);
                write_all();
                return true;
            }
            write_all();
            return false;
        }

        Value at(const Key &key) {
            position p = search(key);
            if (p.first != nullptr){
                Value val;
                fseek(val_file, p.first->info[p.second].second, SEEK_SET);
                fread(&val, sizeof(Value), 1, val_file);
                write_all();
                return val;
            }
            write_all();
            return Value();
        }

        bool find(const Key &key) {
            position p = search(key);
            return p.first != nullptr;
        }

        bool end(){
            return false;
        }

        bool erase(const Key &key) {
            if (basic->siz == 0) return false;
            position pos = search(key);
            if (pos.first == nullptr){
                write_all();
                return false;
            }
            --basic->siz;
            Node *cur_node = pos.first;
            size_t cur_pos = pos.second;
            size_t sz = --cur_node->siz;
            basic->free_pos2[++basic->free_num2] = cur_node->info[cur_pos].second;
            for(size_t i = cur_pos; i <= sz; ++i){
                cur_node->info[i] = cur_node->info[i + 1];
            }
            if(cur_node == root){
                return true;
            }
            if(cur_pos == 1) {
                modify_inner(cur_node->par, cur_node->info[1].first);
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
