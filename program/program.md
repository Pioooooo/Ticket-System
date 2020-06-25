### 说明

```c++
struct user {char username[21],password[31],name[16],mailAddr[31];int privilege;};
```
储存用户信息
```c++
struct train{char trainID[21],stations[100][31],type[2];int stationNum,seatNum,seatNumId[93],prices[100],startTime[2],travelTimes[100],stopoverTimes[100],saleDate[4];int release;};
```
储存列车信息，其中seatNumId用于指向储存当天座位数量的key
```c++
struct seatNum{int num[100];};
```
储存当天座位数量
```c++
struct station{unsigned int trainbit[500];};
```
储存车站有哪些车经过，类似bitset
```c++
int timestamp;
```
时间戳，0代表2020-01-01 00:00，单位为分钟
```c++
struct ticket{char trainID[21];int leaving_time,arriving_time,price,seat;};
```
储存车票信息
```c++
struct order{char trainID[21],from[31],to[31];int status,leaving_time,arriving_time,price,num;int seatid,startstationid,endstationid;};
```
储存订单信息
```c++
struct order_list{int id[50],size;unsigned long long nxt;};
```
储存订单列表，类似块状链表
```c++
BTree<unsigned long long, user> user_table;
```
key为username的hash值
```c++
map<unsigned long long, bool> session;
```
保存登录状态，key为username的hash值
```c++
BTree<unsigned long long, train> train_table;
```
key为trainID的hash值
```c++
BTree<int, seatNum> seat_table;
```
key为seatNumId的值，seatNumId自增
```c++
BTree<int, unsigned long long> release_table;
```
用于储存已经release的车辆的trainID的hash值，key自增
```c++
BTree<unsigned long long, station> station_table;
```
key为车站名称的hash值
```c++
BTree<int, order> order_table;
```
key自增
```c++
BTree<unsigned long long, order_list> user_order_table;
```
key为username的hash值
```c++
BTree<unsigned long long, order_list> train_order_table;
```
key为trainID的hash值

### 操作实现

- main函数负责读入每个操作
- add_user，读入，判断权限是否合法，在user_table中添加
- login，读入，与user_table中的比较，在session中添加
- logout，读入，在session中删除
- query_profile，读入，判断权限是否合法，在user_table中查询
- modify_profile，读入，判断权限是否合法，在user_table中修改
- add_train，读入，在train_table中添加
- release_train，读入，在train_table中修改为release，在release_table中添加，在seat_table中创建每一天的座位，在train_order_table中创建空列表
- query_train，读入，在train_table中查询，在seat_table中查询座位信息
- delete_train，读入，在train_table中删除
- query_ticket，读入，在station_table中查询同时经过始发站和终点站的车次，遍历这些车次
- query_transfer，读入，在station_table中查询经过始发站的车次，遍历这些车次，把经过的每一个站都当为中转站进行尝试，保留合法的
- buy_ticket，读入，在seat_table中修改座位数，在order_table、user_order_table、train_order_table中添加订单信息
- query_order，读入，在user_order_table中查询后在order_table中查询
- refund_ticket，读入，在user_order_table中查询后在order_table中修改，并通过train_order_table找出该车次的候补订单尝试购买
- clean，清除数据
- exit，退出