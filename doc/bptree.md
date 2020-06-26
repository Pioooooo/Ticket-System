```c++
class BTree<class Key, class Value, class Compare = std::less<Key>>
```


### 说明

- 封装好的bptree
- 文件操作和 cache 优化内嵌在这个 class 中
- 没实现顺序访问，因为此次大作业并不需要



### 内嵌类

- ```c++
  class basic_info
  ```

  记录bptree的一些关键信息

  - ```
    off_t root
    ```

    根节点在文件中的位置

  - ```
    size_t siz
    ```

    整棵bptree中储存信息的条数

  - ```
    size_t free_num
    off_t free_pos[200]
    ```

    储存节点的文件中，未被使用的块的数目及其位置

  - ```
    size_t free_num2
    off_t free_pos2[200]
    ```

    储存Value的文件中未被使用的块的数目及其位置



- ```c++
  class Node
  ```
  
  根节点叶节点统一使用这个类

  - ```
    pair<Node *, size_t> par
    ```
    par.Node *指向该节点的父节点在内存中的地址；par.size_t 记录它是父节点的第几个儿子

    只在将节点读入内存的时候维护par
  - ```
    off_t pos
    ```

    该节点在文件中的位置

  - ```
    bool is_leaf
    ```

    标记是否为叶节点

  - ```
    size_t siz
    ```
    
    节点中储存的key值的个数
    
  - ```
    typedef pair<Key, off_t> sub_node;
    sub_node info[MAX_SIZ + 1]
    ```

    非叶子节点：存储儿子节点的Key值及儿子节点在文件中的位置

    叶子节点：存储Value的Key值及Value在文件中的位置



### Cache

- ```
  static const size_t pool_siz = 997;
  Node *pool[pool_siz];
  size_t num = 0;
  size_t occupied[pool_siz]	//num每加一，就记录该节点在pool中的下标
  ```

  节点读入内存时依据 off_t 的值 hash 到 pool 中 

  num 记录 pool 中已经存进来的节点数，当num大于某一给定值后，内存中所有节点全部写入磁盘

- 相关成员函数：

  ```c++
  void _write(Node **p){}			//将一个节点写回磁盘
  void write_all(){}				//当num大于某一给定值后，将内存中所有节点全部写入磁盘
  								//每个公有成员函数return前都调用write_all()
  Node **_read(const off_t pos){}	//将位于pos的节点读入内存，并hash到pool中
  ```



### 私有成员函数

- ```c++
  typedef pair<Node *, size_t> position;
  position search(const Key &key){}
  ```

  从root出发找到key所在节点及其在节点中的位置，找不到就返回(null, 0)

  同时维护路径上所有节点中的par

- ```c++
  position search_insert (const Key &key){}
  ```

  从root出发找到bptree中比key小的最大元素所在的位置

  bptree中已经有key则返回 (null, 0)

  同时维护路径上所有节点的par

- ```c++
  void split_leaf(Node *pos){}
  ```

  将叶节点pos分裂成两个节点，并在pos.par后面插入新节点的Key值和off_t值

  若pos是root，则生成一个新的root，其两个儿子分别是pos和新节点

- ```c++
  void split_inner(Node *pos){}
  ```

  将非叶节点pos分裂成两个节点，并在pos.par后面插入新节点的Key值和off_t值

  若pos是root，则生成一个新的root，其两个儿子分别是pos和新节点

- ```c++
  void insert_inner(position pre, off_t &s, Key &key){}
  ```

  在非叶节点pre.first的pre.second位置插入Key值为key、文件中位置为s的新节点

  如果节点的siz大于MAX_SIZ，调用split_inner

- ```c++
  void modify_inner(position &p, Key &key){}
  ```

  将非叶节点位置p处的Key值修改为key

- ```c++
  void merge_leaf(Node *pos){}
  ```

  当叶节点pos的兄弟节点存有的信息条数大于MIN_SIZ时，从兄弟节点处拿一条信息存到自己这，并调用modify_inner

  否则就合并这两个节点，从父节点那里删去被删节点的信息

  父节点是root且只有一个Key时，merge出的节点成为新的根节点

- ```c++
  void merge_inner(Node *pos){}
  ```

  当非叶节点pos的兄弟节点的儿子数大于MIN_SIZ时，从兄弟节点处拿一个儿子到自己这，并调用modify_inner

  否则就合并这两个节点，从父节点那里删去被删节点的信息

  父节点是root且只有一个Key时，merge出的节点成为新的根节点

- ```c++
  void erase_inner(position &pos){}
  ```

  在非叶节点pos.first中删去第pos.second个儿子的信息
如果该节点儿子数少于MIN_SIZ就调用merge_inner
  
  
### 公有成员函数

- ```c++
  explicit BTree(const char *file_name1 = "data1", const char *file_name2 = "data2"){}
  ```

  没有名为“data1”的文件就创建一个名为“data1”的文件，并构造空的bptree

  否则就读取文件中的信息，构造出bptree

- ```c++
  ~BTree(){}
  ```

  把内存中的basic和所有节点都写回磁盘，关闭两个文件流

- ```c++
  void clear(){}
  ```

  清空bptree的所有信息，构造空的bptree

- ```c++
  size_t size(){}
  ```

  返回bptree中存的信息总数

- ```c++
  bool insert(const Key &key, const Value &value){}
  ```

  用search_insert找到对应的叶节点和位置，插入key值和value所在位置的off_t值

  如果节点的siz大于MAX_SIZ，调用split_leaf

  如果key为所在节点的第一个Key值，调用modify_inner

- ```c++
  bool modify(const Key &key, const Value &value){}
  ```

  用search到key所在位置，用value覆盖该处原来的Value

- ```c++
  Value at(const Key &key){}
  ```

  用search找到key所在位置，读取对应的value并返回；

  如果没有找到key就返回Value()

- ```c++
  bool find(const Key &key){}
  ```

  bptree里有key就返回true，反之返回false

- ```c++
  bool erase(const Key &key){}
  ```

  用search找到key所在位置并删除相应信息

  如果该叶节点存储信息条数小于MIN_SIZ，调用merge_leaf

  如果key是所在叶节点的第一个Key值，调用modify_inner