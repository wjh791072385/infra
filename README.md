# infra
c/c++infrastructure

done:

- [ ] async job queue

- [x] skiplist    
- [ ] threadpool
- [ ] semephore + semctx
- [ ] rwlock



todo:

- [ ] skiplist + comparator + iterator 
- [ ] STL
- [ ] heap



#### 跳表skiplist

|             | 10w insert +  10w search | 100w insert + 100w search |
| ----------- | ------------------------ | ------------------------- |
| skiplist_v1 | 0.157974s                | 3.8727s                   |



#### 读写锁rwlock

临界区读操作为遍历长度10000的vector<br>
临界区写操作为向vector中追加一个数字

|               | 4线程20w次读2000次写 | 4线程10w次读10w次写 |
| ------------- | -------------------- | ------------------- |
| mutex         | 20.1295s             | 8.48541             |
| rwlock_wfirst | 9.09972s             | 8.79313             |
| rwlock_rfirst | 8.70412s             | 4.46938             |
| rwlock_boost  | 9.92822s             | 8.13775             |
