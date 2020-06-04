//
//  main.cpp
//  ticketsystem
//
//  Created by apple on 2020/5/13.
//  Copyright © 2020 apple. All rights reserved.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <random>
#include "../btree/utility.hpp"
#include "../btree/bptree.hpp"
#include "vector.hpp"
#include "map.hpp"

struct user {
    char username[21],password[31],name[16],mailAddr[31];
    int privilege;
    user(){privilege=-1;}
};
sjtu::BTree<unsigned long long, user> user_table("user_table.data","user_table.data2");
sjtu::map<unsigned long long, bool> session;
struct train{
    char trainID[21],stations[100][31],type[2];
    int stationNum,seatNumId[93],prices[100],startTime[2],travelTimes[100],stopoverTimes[100],saleDate[4];
    int release;
    train(){release=-1;}
};
sjtu::BTree<unsigned long long, train> train_table("train_table.data","train_table.data2");
struct seatNum{int num[100];};
sjtu::BTree<int, seatNum> seat_table("seat_table.data","seat_table.data2");
sjtu::BTree<int, unsigned long long> release_table("release_table.data","release_table.data2");
struct station{
    unsigned int trainbit[233];
    station(){memset(trainbit,0,sizeof(trainbit));}
    inline void set(int p){
        trainbit[p>>5]=trainbit[p>>5]|(1<<(p%32-1));
    }
    inline int get(int p){
        return (trainbit[p>>5]>>(p%32))&1;
    }
};
sjtu::BTree<unsigned long long, station> station_table("station_table.data","station_table.data2");
inline int calctimestamp(int month,int day,int hour=0,int minute=0){
    // 01-01 00:00 -> 0
    static const int bl[13]={0,0,31,31+29,31+29+31,31+29+31+30,31+29+31+30+31,31+29+31+30+31+30,31+29+31+30+31+30+31,31+29+31+30+31+30+31+31,31+29+31+30+31+30+31+31+30,31+29+31+30+31+30+31+31+30+31,31+29+31+30+31+30+31+31+30+31+30};
    int ans=(bl[month]+day-1)*1440+hour*60+minute;
    return ans;
}
struct datetime{
    int month,day,hour,minute;
    datetime(int m,int d,int h,int mm){month=m;day=d;hour=h;minute=mm;}
};
inline datetime rcalctimestamp(int timestamp){
    static const int bl[13]={0,0,31,31+29,31+29+31,31+29+31+30,31+29+31+30+31,31+29+31+30+31+30,31+29+31+30+31+30+31,31+29+31+30+31+30+31+31,31+29+31+30+31+30+31+31+30,31+29+31+30+31+30+31+31+30+31,31+29+31+30+31+30+31+31+30+31+30};
    int day=timestamp/1440+1,minute=timestamp%1440;
    int hour=minute/60;minute%=60;
    int month=0;
    for (int i=1;i<=12;i++) if (day>bl[i]) month=i;
    day-=bl[month];
    return datetime(month,day,hour,minute);
}
struct ticket{
    char trainID[21];int leaving_time,arriving_time,price,seat;
    ticket(){leaving_time=0;arriving_time=233333333;price=233333333;}
};
struct order{
    char trainID[21],from[31],to[31];int status,leaving_time,arriving_time,price,num;
    int seatid,startstationid,endstationid;
    // status 1->success 2->pending 3->refunded
};
sjtu::BTree<int, order> order_table("order_table.data","order_table.data2");
struct order_list{
    int id[50],size;unsigned long long nxt;
    order_list(){size=0;nxt=0;}
};
std::random_device rd;std::mt19937_64 gen(rd());
std::uniform_int_distribution<unsigned long long> rnd;
sjtu::BTree<unsigned long long, order_list> user_order_table("user_order_table.data","user_order_table.data2");
sjtu::BTree<unsigned long long, order_list> train_order_table("train_order_table.data","train_order_table.data2");
inline void user_order_add(unsigned long long x,int id){
    order_list p=user_order_table.at(x);unsigned long long tp=x;
    while (p.size>=50){
        if (p.nxt!=0) {tp=p.nxt;p=user_order_table.at(p.nxt);}
        else{
            p.nxt=rnd(gen);user_order_table.modify(tp,p);
            tp=p.nxt;p=order_list();
            user_order_table.insert(tp,p);
        }
    }
    p.id[p.size++]=id;
    user_order_table.modify(tp,p);
}
sjtu::vector<order_list> user_order_get(unsigned long long x){
    sjtu::vector<order_list> ans;
    order_list p=user_order_table.at(x);
    ans.push_back(p);
    while (p.nxt!=0){
        p=user_order_table.at(p.nxt);
        ans.push_back(p);
    }
    return ans;
}
inline void train_order_add(unsigned long long x,int id){
    order_list p=train_order_table.at(x);unsigned long long tp=x;
    while (p.size>=50){
        if (p.nxt!=0) {tp=p.nxt;p=train_order_table.at(p.nxt);}
        else{
            p.nxt=rnd(gen);train_order_table.modify(tp,p);
            tp=p.nxt;p=order_list();
            train_order_table.insert(tp,p);
        }
    }
    p.id[p.size++]=id;
    train_order_table.modify(tp,p);
}
sjtu::vector<order_list> train_order_get(unsigned long long x){
    sjtu::vector<order_list> ans;
    order_list p=train_order_table.at(x);
    ans.push_back(p);
    while (p.nxt!=0){
        p=train_order_table.at(p.nxt);
        ans.push_back(p);
    }
    return ans;
}

const unsigned long long w=100007;
inline unsigned long long hash(const char * str){
    int len=(int)strlen(str);unsigned long long ans=0;
    for (int i=0;i<len;i++) ans=ans*w+str[i];
    return ans;
}
inline char getarg(){
    char c=getchar();
    while (!(c=='-' || c=='\n' || c=='\r')) c=getchar();
    if (c=='-'){
        c=getchar();
        return c;
    }else{
        return '\n';
    }
}
inline void add_user(){
    char c;
    char cur_username[21];
    user new_user;
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'c':scanf("%s",cur_username);break;
            case 'u':scanf("%s",new_user.username);break;
            case 'p':scanf("%s",new_user.password);break;
            case 'n':scanf("%s",new_user.name);break;
            case 'm':scanf("%s",new_user.mailAddr);break;
            case 'g':scanf("%d",&new_user.privilege);break;
            default:break;
        }
    }
    if (user_table.size()==0){
        new_user.privilege=10;
        user_table.insert(hash(new_user.username), new_user);
        user_order_table.insert(hash(new_user.username), order_list());
        printf("0\n");
    }else{
        if (session.find(hash(cur_username))==session.end()){
            printf("-1\n");
            return;
        }
        user cur_user=user_table.at(hash(cur_username));
        if (cur_user.privilege>new_user.privilege && user_table.find(hash(new_user.username))==user_table.end()){
            user_table.insert(hash(new_user.username), new_user);
            user_order_table.insert(hash(new_user.username), order_list());
            printf("0\n");
        }else{
            printf("-1\n");
        }
    }
}
inline void login(){
    char c;
    char username[21],password[31];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'u':scanf("%s",username);break;
            case 'p':scanf("%s",password);break;
            default:break;
        }
    }
    user user=user_table.at(hash(username));
    if (user.privilege!=-1 && strcmp(user.password,password)==0 && session.find(hash(username))==session.end()){
        session.insert(sjtu::pair<unsigned long long, bool>(hash(username),true));
        printf("0\n");
    }else{
        printf("-1\n");
    }
}
inline void logout(){
    char c;
    char username[21];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'u':scanf("%s",username);break;
            default:break;
        }
    }
    if (session.find(hash(username))!=session.end()){
        session.erase(session.find(hash(username)));
        printf("0\n");
    }else{
        printf("-1\n");
    }
}
inline void query_profile(){
    char c;
    char cur_username[21],username[21];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'c':scanf("%s",cur_username);break;
            case 'u':scanf("%s",username);break;
            default:break;
        }
    }
    if (session.find(hash(cur_username))!=session.end()){
        user cur,user;
        cur=user_table.at(hash(cur_username));
        user=user_table.at(hash(username));
        if (user.privilege!=-1 && (cur.privilege>user.privilege || strcmp(cur_username,username)==0)){
            printf("%s %s %s %d\n",user.username,user.name,user.mailAddr,user.privilege);
        }else{
            printf("-1\n");
        }
    }else{
        printf("-1\n");
    }
}
inline void modify_profile(){
    char c;
    char cur_username[21];
    user new_user;bool hp=false,hn=false,hm=false,hg=false;
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'c':scanf("%s",cur_username);break;
            case 'u':scanf("%s",new_user.username);break;
            case 'p':scanf("%s",new_user.password);hp=true;break;
            case 'n':scanf("%s",new_user.name);hn=true;break;
            case 'm':scanf("%s",new_user.mailAddr);hm=true;break;
            case 'g':scanf("%d",&new_user.privilege);hg=true;break;
            default:break;
        }
    }
    if (session.find(hash(cur_username))!=session.end()){
        user cur,user;
        cur=user_table.at(hash(cur_username));
        user=user_table.at(hash(new_user.username));
        if (user.privilege!=-1 && (cur.privilege>user.privilege || strcmp(cur_username,user.username)==0)){
            if (hp) strcpy(user.password,new_user.password);
            if (hn) strcpy(user.name,new_user.name);
            if (hm) strcpy(user.mailAddr,new_user.mailAddr);
            if (hg) user.privilege=new_user.privilege;
            user_table.modify(hash(user.username),user);
            printf("%s %s %s %d\n",user.username,user.name,user.mailAddr,user.privilege);
        }else{
            printf("-1\n");
        }
    }else{
        printf("-1\n");
    }
}
inline void add_train(){
    char c;
    train train;
    static char temp[3333];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'i':scanf("%s",train.trainID);break;
            case 'n':scanf("%d",&train.stationNum);break;
            case 'm':{
                scanf("%d",&train.seatNumId[0]);
                break;
            }
            case 's':{
                scanf("%s",temp);
                char *p=strtok(temp,"|");int tot=0;
                do{strcpy(train.stations[tot++],p);}while((p=strtok(NULL,"|")));
                break;
            }
            case 'p':{
                for (int i=0;i<train.stationNum-1-1;i++) scanf("%d|",&train.prices[i]);
                scanf("%d",&train.prices[train.stationNum-1-1]);
                train.prices[train.stationNum-1]=0;
                break;
            }
            case 'x':{
                scanf("%d:%d",&train.startTime[0],&train.startTime[1]);
                break;
            }
            case 't':{
                for (int i=0;i<train.stationNum-1-1;i++) scanf("%d|",&train.travelTimes[i]);
                scanf("%d",&train.travelTimes[train.stationNum-1-1]);
                train.travelTimes[train.stationNum-1]=0;
                break;
            }
            case 'o':{
                if (train.stationNum==2) {
                    scanf("%s",temp);
                    break;
                }
                for (int i=0;i<train.stationNum-2-1;i++) scanf("%d|",&train.stopoverTimes[i]);
                scanf("%d",&train.stopoverTimes[train.stationNum-2-1]);
                train.stopoverTimes[train.stationNum-2]=train.stopoverTimes[train.stationNum-1]=0;
                break;
            }
            case 'd':{
                scanf("%d-%d|%d-%d",&train.saleDate[0],&train.saleDate[1],&train.saleDate[2],&train.saleDate[3]);
                break;
            }
            case 'y':scanf("%s",train.type);break;
            default:break;
        }
    }
    train.release=0;
    if (train_table.find(hash(train.trainID))==train_table.end()){
        train_table.insert(hash(train.trainID), train);
        printf("0\n");
    }else{
        printf("-1\n");
    }
}
inline void release_train(){
    char c;
    char trainID[21];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'i':scanf("%s",trainID);break;
            default:break;
        }
    }
    train train=train_table.at(hash(trainID));
    if (train.release==0){
        int id=(int)release_table.size()+1;
        release_table.insert(id,hash(trainID));
        seatNum seat;
        for (int i=0;i<train.stationNum;i++){
            if (station_table.find(hash(train.stations[i]))==station_table.end()){
                station tmp;tmp.set(id);
                station_table.insert(hash(train.stations[i]),tmp);
            }else{
                station tmp=station_table.at(hash(train.stations[i]));tmp.set(id);
                station_table.modify(hash(train.stations[i]),tmp);
            }
            seat.num[i]=train.seatNumId[0];
        }
        int ss=calctimestamp(train.saleDate[0],train.saleDate[1]),st=calctimestamp(train.saleDate[2],train.saleDate[3]);
        int days=(st-ss)/1440+1;
        for (int i=0;i<days;i++){
            train.seatNumId[i]=(int)seat_table.size()+1;
            seat_table.insert(train.seatNumId[i], seat);
        }
        train.release=1;
        train_table.modify(hash((trainID)), train);
        train_order_table.insert(hash((trainID)), order_list());
        printf("0\n");
    }else{
        printf("-1\n");
    }
}
inline void query_train(){
    char c;
    char trainID[21];int date[2];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'i':scanf("%s",trainID);break;
            case 'd':scanf("%d-%d",&date[0],&date[1]);
            default:break;
        }
    }
    train train=train_table.at(hash(trainID));
    if (train.release!=-1){
        int tmp=calctimestamp(date[0],date[1]),ss=calctimestamp(train.saleDate[0],train.saleDate[1]),st=calctimestamp(train.saleDate[2],train.saleDate[3]);
        if (tmp<ss || tmp>st){
            printf("-1\n");
            return;
        }
        printf("%s %s\n",train.trainID,train.type);
        int timestamp=calctimestamp(date[0],date[1],train.startTime[0],train.startTime[1]),price=0;
        seatNum seat;bool bs=false;
        if (train.release==1) seat=seat_table.at(train.seatNumId[(tmp-ss)/1440]);
        else bs=true;
        for (int i=0;i<train.stationNum;i++){
            printf("%s ",train.stations[i]);
            if (i==0) printf("xx-xx xx:xx");
            else{
                datetime t=rcalctimestamp(timestamp);
                printf("%02d-%02d %02d:%02d",t.month,t.day,t.hour,t.minute);
                timestamp+=train.stopoverTimes[i-1];
            }
            printf(" -> ");
            if (i==train.stationNum-1) printf("xx-xx xx:xx");
            else{
                datetime t=rcalctimestamp(timestamp);
                printf("%02d-%02d %02d:%02d",t.month,t.day,t.hour,t.minute);
                timestamp+=train.travelTimes[i];
            }
            printf(" %d ",price);price+=train.prices[i];
            if (i==train.stationNum-1) printf("x");
            else{
                if (bs) printf("%d",train.seatNumId[0]);
                else printf("%d",seat.num[i]);
            }
            printf("\n");
        }
    }else{
        printf("-1\n");
    }
}
inline void delete_train(){
    char c;
    char trainID[21];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'i':scanf("%s",trainID);break;
            default:break;
        }
    }
    train train=train_table.at(hash(trainID));
    if (train.release==0){
        train_table.erase(hash(trainID));
        printf("0\n");
    }else{
        printf("-1\n");
    }
}
inline void query_ticket(){
    char c;
    char startstation[31],endstation[31];int date[2];bool hp=false;char p[5];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 's':scanf("%s",startstation);break;
            case 't':scanf("%s",endstation);break;
            case 'd':scanf("%d-%d",&date[0],&date[1]);break;
            case 'p':hp=true;scanf("%s",p);break;
            default:break;
        }
    }
    station ss=station_table.at(hash(startstation)),st=station_table.at(hash(endstation));
    static sjtu::vector<ticket> ans;ans.clear();
    static sjtu::vector<int> ansid;ansid.clear();
    for (int i=0;i<233;i++) {
        unsigned int tmp=ss.trainbit[i]&st.trainbit[i],id=i*32+1;
        while (tmp>0){
            if (tmp&1){
                train train=train_table.at(release_table.at(id));
                int timestamp=calctimestamp(train.saleDate[0],train.saleDate[1],train.startTime[0],train.startTime[1]),price=0,seat=23333333;
                int sta=-1,done=-1;
                int starttimestamp=0,endtimestamp=0;
                for (int i=0;i<train.stationNum;i++){
                    if (strcmp(startstation,train.stations[i])==0){
                        sta=i;
                        starttimestamp=timestamp;
                    }
                    if (strcmp(endstation,train.stations[i])==0){
                        if (sta>=0) done=i;else break;
                        endtimestamp=timestamp-train.stopoverTimes[i-1];
                        break;
                    }
                    timestamp+=train.travelTimes[i]+train.stopoverTimes[i];
                    if (sta>=0) {price+=train.prices[i];}
                }
                if (done>=0){
                    datetime temp=rcalctimestamp(starttimestamp);
                    int ts=calctimestamp(temp.month,temp.day),tt=ts+(calctimestamp(train.saleDate[2],train.saleDate[3])-calctimestamp(train.saleDate[0],train.saleDate[1]));
                    int tb=calctimestamp(date[0],date[1]);
                    if (ts<=tb && tb<=tt){
                        starttimestamp+=tb-ts;
                        endtimestamp+=tb-ts;
                        seatNum seatNum=seat_table.at(train.seatNumId[(tb-ts)/1440]);
                        for (int i=sta;i<done;i++) seat=std::min(seat,seatNum.num[i]);
                        ticket ticket;
                        strcpy(ticket.trainID,train.trainID);
                        ticket.leaving_time=starttimestamp;ticket.arriving_time=endtimestamp;
                        ticket.price=price;ticket.seat=seat;
                        ans.push_back(ticket);ansid.push_back((int)ansid.size());
                    }
                }
            }
            tmp>>=1;id++;
        }
    }
    if (hp && p[0]=='t') std::sort(ansid.begin(),ansid.end(),[](const int &a, const int &b){return (ans[a].arriving_time-ans[a].leaving_time)<(ans[b].arriving_time-ans[b].leaving_time);});
    else std::sort(ansid.begin(),ansid.end(),[](const int &a, const int &b){return ans[a].price<ans[b].price;});
    printf("%d\n",(int)ans.size());
    for (int i=0;i<ans.size();i++){
        datetime datetime1=rcalctimestamp(ans[ansid[i]].leaving_time),datetime2=rcalctimestamp(ans[ansid[i]].arriving_time);
        printf("%s %s %02d-%02d %02d:%02d -> %s %02d-%02d %02d:%02d %d %d\n",ans[ansid[i]].trainID,startstation,datetime1.month,datetime1.day,datetime1.hour,datetime1.minute,endstation,datetime2.month,datetime2.day,datetime2.hour,datetime2.minute,ans[ansid[i]].price,ans[ansid[i]].seat);
    }
}
inline void query_transfer(){
    char c;
    char startstation[31],endstation[31];int date[2];bool hp=false;char p[5];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 's':scanf("%s",startstation);break;
            case 't':scanf("%s",endstation);break;
            case 'd':scanf("%d-%d",&date[0],&date[1]);break;
            case 'p':hp=true;scanf("%s",p);break;
            default:break;
        }
    }
    station ss=station_table.at(hash(startstation)),st=station_table.at(hash(endstation));
    ticket ans1,ans2;char midstation[31];
    for (int i=0;i<233;i++) {
        unsigned int tmp=ss.trainbit[i],id=i*32+1;
        while (tmp>0){
            if (tmp&1){
                train train=train_table.at(release_table.at(id));
                int timestamp=calctimestamp(train.saleDate[0],train.saleDate[1],train.startTime[0],train.startTime[1]);
                int sta=-1;
                int starttimestamp=0;
                for (int i=0;i<train.stationNum;i++){
                    if (strcmp(startstation,train.stations[i])==0){
                        sta=i;starttimestamp=timestamp;break;
                    }
                    timestamp+=train.travelTimes[i]+train.stopoverTimes[i];
                }
                datetime temp=rcalctimestamp(starttimestamp);
                int ts=calctimestamp(temp.month,temp.day),tt=ts+(calctimestamp(train.saleDate[2],train.saleDate[3])-calctimestamp(train.saleDate[0],train.saleDate[1]));
                int tb=calctimestamp(date[0],date[1]);
                if (ts<=tb && tb<=tt){
                    starttimestamp+=tb-ts;timestamp+=tb-ts;
                    seatNum seatNum=seat_table.at(train.seatNumId[(tb-ts)/1440]);
                    int price=0,seat=23333333;
                    for (int i=sta;i<train.stationNum;i++){
                        if (i>sta && strcmp(train.stations[i],endstation)!=0){
                            {
                                char station3[31];strcpy(station3,train.stations[i]);
                                station s3=station_table.at(hash(station3));
                                for (int i=0;i<233;i++) {
                                    unsigned int tmp=s3.trainbit[i]&st.trainbit[i],id=i*32+1;
                                    while (tmp>0){
                                        if (tmp&1){
                                            struct train train2=train_table.at(release_table.at(id));
                                            int timestamp2=calctimestamp(train2.saleDate[0],train2.saleDate[1],train2.startTime[0],train2.startTime[1]),price2=0,seat2=23333333;
                                            int sta=-1,done=-1;
                                            int starttimestamp2=0,endtimestamp2=0;
                                            for (int i=0;i<train2.stationNum;i++){
                                                if (strcmp(station3,train2.stations[i])==0){
                                                    sta=i;
                                                    starttimestamp2=timestamp2;
                                                }
                                                if (strcmp(endstation,train2.stations[i])==0){
                                                    if (sta>=0) done=i;else break;
                                                    endtimestamp2=timestamp2-train2.stopoverTimes[i-1];
                                                    break;
                                                }
                                                timestamp2+=train2.travelTimes[i]+train2.stopoverTimes[i];
                                                if (sta>=0) {price2+=train2.prices[i];}
                                            }
                                            if (done>=0){
                                                timestamp2=calctimestamp(train2.saleDate[0],train2.saleDate[1],train2.startTime[0],train2.startTime[1]);
                                                datetime temp=rcalctimestamp(starttimestamp2);
                                                int ts=calctimestamp(temp.month,temp.day),tt=ts+(calctimestamp(train2.saleDate[2],train2.saleDate[3])-calctimestamp(train2.saleDate[0],train2.saleDate[1]));
                                                datetime tmp2=rcalctimestamp(timestamp);
                                                int tb=calctimestamp(tmp2.month,tmp2.day);
                                                if (starttimestamp2+tb-ts<timestamp) tb+=1440;
                                                if (tb<ts) tb=ts;
                                                if (ts<=tb && tb<=tt){
                                                    starttimestamp2+=tb-ts;
                                                    endtimestamp2+=tb-ts;
                                                    struct seatNum seatNum=seat_table.at(train2.seatNumId[(tb-ts)/1440]);
                                                    for (int i=sta;i<done;i++) seat2=std::min(seat2,seatNum.num[i]);
                                                    ticket ticket1,ticket2;
                                                    strcpy(ticket1.trainID,train.trainID);
                                                    ticket1.leaving_time=starttimestamp;ticket1.arriving_time=timestamp;
                                                    ticket1.price=price;ticket1.seat=seat;
                                                    strcpy(ticket2.trainID,train2.trainID);
                                                    ticket2.leaving_time=starttimestamp2;ticket2.arriving_time=endtimestamp2;
                                                    ticket2.price=price2;ticket2.seat=seat2;
                                                    if (hp && p[0]=='t'){
                                                        if (ticket2.arriving_time-ticket1.leaving_time<ans2.arriving_time-ans1.leaving_time){
                                                            ans1=ticket1;ans2=ticket2;
                                                            strcpy(midstation,station3);
                                                        }
                                                    }else{
                                                        if (ticket1.price+ticket2.price<ans1.price+ans2.price){
                                                            ans1=ticket1;ans2=ticket2;
                                                            strcpy(midstation,station3);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        tmp>>=1;id++;
                                    }
                                }
                                ///
                            }
                            timestamp+=train.stopoverTimes[i-1];
                        }
                        timestamp+=train.travelTimes[i];
                        price+=train.prices[i];seat=std::min(seat,seatNum.num[i]);
                    }
                }
            }
            tmp>>=1;id++;
        }
    }
    if (ans1.price!=233333333){
        datetime datetime1=rcalctimestamp(ans1.leaving_time),datetime2=rcalctimestamp(ans1.arriving_time);
        printf("%s %s %02d-%02d %02d:%02d -> %s %02d-%02d %02d:%02d %d %d\n",ans1.trainID,startstation,datetime1.month,datetime1.day,datetime1.hour,datetime1.minute,midstation,datetime2.month,datetime2.day,datetime2.hour,datetime2.minute,ans1.price,ans1.seat);
        datetime1=rcalctimestamp(ans2.leaving_time);datetime2=rcalctimestamp(ans2.arriving_time);
        printf("%s %s %02d-%02d %02d:%02d -> %s %02d-%02d %02d:%02d %d %d\n",ans2.trainID,midstation,datetime1.month,datetime1.day,datetime1.hour,datetime1.minute,endstation,datetime2.month,datetime2.day,datetime2.hour,datetime2.minute,ans2.price,ans2.seat);
    }else{
        printf("0\n");
    }
}
inline void buy_ticket(){
    char c;
    char username[21],trainID[21],startstation[31],endstation[31],q[6];int date[2],num=0;bool hq=false;
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'u':scanf("%s",username);break;
            case 'i':scanf("%s",trainID);break;
            case 'd':scanf("%d-%d",&date[0],&date[1]);break;
            case 'n':scanf("%d",&num);break;
            case 'f':scanf("%s",startstation);break;
            case 't':scanf("%s",endstation);break;
            case 'q':hq=true;scanf("%s",q);break;
            default:break;
        }
    }
    if (session.find(hash(username))==session.end()){printf("-1\n");return;}
    train train=train_table.at(hash(trainID));
    if (train.release==-1){printf("-1\n");return;}
    int timestamp=calctimestamp(train.saleDate[0],train.saleDate[1],train.startTime[0],train.startTime[1]),price=0,seat=23333333;
    int sta=-1,done=-1;
    int starttimestamp=0,endtimestamp=0;
    for (int i=0;i<train.stationNum;i++){
        if (strcmp(startstation,train.stations[i])==0){
            sta=i;
            starttimestamp=timestamp;
        }
        if (strcmp(endstation,train.stations[i])==0){
            if (sta>=0) done=i;else break;
            endtimestamp=timestamp-train.stopoverTimes[i-1];
            break;
        }
        timestamp+=train.travelTimes[i]+train.stopoverTimes[i];
        if (sta>=0) {price+=train.prices[i];}
    }
    if (done>=0){
        datetime temp=rcalctimestamp(starttimestamp);
        int ts=calctimestamp(temp.month,temp.day),tt=ts+(calctimestamp(train.saleDate[2],train.saleDate[3])-calctimestamp(train.saleDate[0],train.saleDate[1]));
        int tb=calctimestamp(date[0],date[1]);
        if (ts<=tb && tb<=tt){
            starttimestamp+=tb-ts;
            endtimestamp+=tb-ts;
            seatNum seatNum=seat_table.at(train.seatNumId[(tb-ts)/1440]);
            for (int i=sta;i<done;i++) seat=std::min(seat,seatNum.num[i]);
            ticket ticket;
            strcpy(ticket.trainID,train.trainID);
            ticket.leaving_time=starttimestamp;ticket.arriving_time=endtimestamp;
            ticket.price=price;ticket.seat=seat;
            if (ticket.seat>=num){
                for (int i=sta;i<done;i++) seatNum.num[i]-=num;
                seat_table.modify(train.seatNumId[(tb-ts)/1440],seatNum);
                order order;
                strcpy(order.trainID,ticket.trainID);
                strcpy(order.from,startstation);
                strcpy(order.to,endstation);
                order.status=1;
                order.leaving_time=ticket.leaving_time;
                order.arriving_time=ticket.arriving_time;
                order.price=ticket.price;
                order.num=num;
                order.seatid=(tb-ts)/1440;order.startstationid=sta;order.endstationid=done;
                int id=(int)order_table.size()+1;
                order_table.insert(id, order);
                user_order_add(hash(username),id);
                train_order_add(hash(trainID),id);
                printf("%d\n",order.price*num);
            }else{
                if (hq && q[0]=='t'){
                    order order;
                    strcpy(order.trainID,ticket.trainID);
                    strcpy(order.from,startstation);
                    strcpy(order.to,endstation);
                    order.status=2;
                    order.leaving_time=ticket.leaving_time;
                    order.arriving_time=ticket.arriving_time;
                    order.price=ticket.price;
                    order.num=num;
                    order.seatid=(tb-ts)/1440;order.startstationid=sta;order.endstationid=done;
                    int id=(int)order_table.size()+1;
                    order_table.insert(id, order);
                    user_order_add(hash(username),id);
                    train_order_add(hash(trainID),id);
                    printf("queue\n");
                }else{
                    printf("-1\n");
                }
            }
        }else{
            printf("-1\n");
        }
    }else{
        printf("-1\n");
    }
}
inline void query_order(){
    char c;
    char username[21];
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'u':scanf("%s",username);break;
            default:break;
        }
    }
    const char status_bl[4][9]={"","success","pending","refunded"};
    if (session.find(hash(username))==session.end()){printf("-1\n");return;}
    sjtu::vector<order_list> ans=user_order_get(hash(username));
    int num=0;
    for (auto x:ans) num+=x.size;
    printf("%d\n",num);
    for (int I=(int)ans.size()-1;I>=0;I--){
        for (int i=ans[I].size-1;i>=0;i--){
            order order=order_table.at(ans[I].id[i]);
            datetime datetime1=rcalctimestamp(order.leaving_time),datetime2=rcalctimestamp(order.arriving_time);
            printf("[%s] %s %s %02d-%02d %02d:%02d -> %s %02d-%02d %02d:%02d %d %d\n",status_bl[order.status],order.trainID,order.from,datetime1.month,datetime1.day,datetime1.hour,datetime1.minute,order.to,datetime2.month,datetime2.day,datetime2.hour,datetime2.minute,order.price,order.num);
        }
    }
}
inline void refund_ticket(){
    char c;
    char username[21];int id=0;
    while ((c=getarg())!='\n') {
        switch (c) {
            case 'u':scanf("%s",username);break;
            case 'n':scanf("%d",&id);break;
            default:break;
        }
    }
    if (session.find(hash(username))==session.end()){printf("-1\n");return;}
    sjtu::vector<order_list> ans=user_order_get(hash(username));
    int realid=0;
    for (int I=(int)ans.size()-1;I>=0;I--){
        if (id>ans[I].size) id-=ans[I].size;
        else{
            realid=ans[I].id[ans[I].size-id];
            break;
        }
    }
    if (realid==0){printf("-1\n");return;}
    order order=order_table.at(realid);
    if (order.status==2){
        order.status=3;
        order_table.modify(realid,order);
        printf("0\n");
        return;
    }
    if (order.status==3){printf("-1\n");return;}
    train train=train_table.at(hash(order.trainID));
    seatNum seatNum=seat_table.at(train.seatNumId[order.seatid]);
    for (int i=order.startstationid;i<order.endstationid;i++) seatNum.num[i]+=order.num;
    seat_table.modify(train.seatNumId[order.seatid],seatNum);
    order.status=3;
    order_table.modify(realid,order);
    printf("0\n");
    ans=train_order_get(hash(train.trainID));
    for (auto x:ans){
        for (int i=0;i<x.size;i++){
            order=order_table.at(x.id[i]);
            if (order.status==2){
                seatNum=seat_table.at(train.seatNumId[order.seatid]);
                int seat=23333333;
                for (int i=order.startstationid;i<order.endstationid;i++) seat=std::min(seat,seatNum.num[i]);
                if (seat>=order.num){
                    for (int i=order.startstationid;i<order.endstationid;i++) seatNum.num[i]-=order.num;
                    seat_table.modify(train.seatNumId[order.seatid],seatNum);
                    order.status=1;
                    order_table.modify(x.id[i],order);
                }
            }
        }
    }
}
inline void clean(){
    user_table.clear();
    session.clear();
    train_table.clear();
    seat_table.clear();
    release_table.clear();
    station_table.clear();
    order_table.clear();
    user_order_table.clear();
    train_order_table.clear();
    printf("0\n");
}
inline void exit(){
    printf("bye\n");
    exit(0);
}
int main(int argc, const char * argv[]) {
    char cmd[23];
    while (true) {
        scanf("%s",cmd);
        /*
         query_profile
         query_ticket
         buy_ticket

         login
         logout
         modify_profile
         query_order

         add_user
         add_train
         release_train
         query_train
         delete_train
         query_transfer
         refund_ticket

         clean
         exit
         */
        if (cmd[0]=='q'){
            if (cmd[9]=='f'){
                query_profile();
            }else if (cmd[9]=='k'){
                query_ticket();
            }else if (cmd[9]=='e'){
                query_order();
            }else if (cmd[9]=='i'){
                query_train();
            }else if (cmd[9]=='n'){
                query_transfer();
            }
        }else if (cmd[0]=='b'){
            buy_ticket();
        }else if (cmd[0]=='l'){
            if (cmd[3]=='i'){
                login();
            }else if (cmd[3]=='o'){
                logout();
            }
        }else if (cmd[0]=='m'){
            modify_profile();
        }else if (cmd[0]=='a'){
            if (cmd[4]=='u'){
                add_user();
            }else if (cmd[4]=='t'){
                add_train();
            }
        }else if (cmd[0]=='r'){
            if (cmd[2]=='l'){
                release_train();
            }else if (cmd[2]=='f'){
                refund_ticket();
            }
        }else if (cmd[0]=='d'){
            delete_train();
        }else if (cmd[0]=='c'){
            clean();
        }else if (cmd[0]=='e'){
            exit();
        }
    }
    return 0;
}
