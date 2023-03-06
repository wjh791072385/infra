#include <ctime>
#include <cstring>
#include <random>

template <typename K, typename V>
class skiplist_node
{
  public:
    skiplist_node(K k, V v, int level);

    ~skiplist_node();

  public:
    K key;
    V value;
    int node_level;
    skiplist_node<K, V> **forward;  //前进指针
};

template <typename K, typename V>
skiplist_node<K, V>::skiplist_node(K k, V v, int level)
    : key(k), value(v), node_level(level) {
        // 最底层为0层，上面有level层索引，下标范围[0...level]
        forward = new skiplist_node<K, V> *[level + 1];
        memset(forward, 0, sizeof(skiplist_node<K, V> *) * (level + 1));
    }

template <typename K, typename V>
skiplist_node<K, V>::~skiplist_node() {
    delete []forward;
}

/*******************************************************************************
*******************************************************************************/

template <typename K, typename V>
class skiplist
{
  public:
    skiplist(int max_level);

    ~skiplist();

    int random_level();

    // 后续可从内存池中进行分配
    skiplist_node<K, V>* create_node(K, V, int level);

    int insert_element(K k, V v);

    skiplist_node<K, V>* search_element(K k);

    int delete_element(K k);

    void display_list();

    int size();

  public:
    // max level of skiplist
    int max_level;

    // current level of skiplist
    int current_level;

    skiplist_node<K, V> *header;

    // skiplist current elemenet count
    int element_count;
};

template <typename K, typename V>
skiplist<K, V>::skiplist(int max_level)
    : max_level(max_level)
{
    this->current_level = 0;
    this->element_count = 0;

    // initialize header
    K k;
    V v;
    this->header = new skiplist_node<K, V>(k, v, max_level);
}

template <typename K, typename V>
skiplist<K, V>::~skiplist()
{
   delete header;
}

template <typename K, typename V>
int skiplist<K, V>::random_level()
{
    int random_level = 1;
	static std::default_random_engine e(time(0));

    //以25%的概率增加层数，参考leveldb
	static std::uniform_int_distribution<int> u(0, 3);

	while (u(e) && random_level < this->max_level)
	{
		random_level++;
	}

	return random_level;
}


template <typename K, typename V>
skiplist_node<K, V>* skiplist<K, V>::create_node(const K k, const V v, int level)
{
    //todo : create node from mempool
    return new skiplist_node<K, V>(k, v, level);
}


/*
                           +------------+
                           |  search 60 |
                           +------------+
level 4     +-->1+                                                      100
                 |
                 |
level 3         1+-------->10+------------------>50+           70       100
                                                   |
                                                   |
level 2         1          10         30         50|           70       100
                                                   |
                                                   |
level 1         1    4     10         30         50|           70       100
                                                   |
                                                   |
level 0         1    4   9 10         30   40    50+-->60      70       100
*/

template <typename K, typename V>
int skiplist<K, V>::insert_element(K k, V v)
{
    skiplist_node<K, V> *cur = this->header;
    skiplist_node<K, V> *update[max_level + 1];
    memset(update, 0, sizeof(skiplist_node<K, V> *) * (max_level + 1));

    //从上往下寻找
    for (int i = this->current_level; i >= 0; i--) {
        while (cur->forward[i] != nullptr && cur->forward[i]->key < k) {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }

    //遍历结束后，cur指向最底层插入位置的前一个node
    //level 0包含了全部元素，如果该元素存在，那么下一个元素就是k，进行更新
    cur = cur->forward[0];
    if (cur != nullptr && cur->key == k) {
        //todo log
        std::cout << "key : " << k << " exists" << std::endl;
        cur->value = v;
        return 1;
    }

    //插入元素
    int rd_level = random_level();

    //如果level > max_level，那么将新元素插入到头结点后
    if (rd_level > this->current_level) {
        for (int i = this->current_level + 1; i <= rd_level; i++) {
            update[i] = this->header;
        }
        current_level = rd_level;
    }

    skiplist_node<K, V> *nd = create_node(k, v, rd_level);
    for (int i = rd_level; i >= 0; i--) {
        nd->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = nd;
    }

    this->element_count++;

    return 0;
}


template <typename K, typename V>
skiplist_node<K, V>* skiplist<K, V>::search_element(K k)
{
    skiplist_node<K, V> *cur = this->header;

    // 从上往下找
    for (int i = this->current_level; i >= 0; i--) {
        while (cur->forward[i] != nullptr && cur->forward[i]->key < k) {
            cur = cur->forward[i];
        }
    }

    cur = cur->forward[0];
    if (cur != nullptr && cur->key == k) {
        std::cout << "key : " << k << " exists" << std::endl;
        return cur;
    }
    std::cout << "key : " << k << " not exists" << std::endl;
    return nullptr;
}


template <typename K, typename V>
int skiplist<K, V>::delete_element(K k)
{
    skiplist_node<K, V> *cur = this->header;
    skiplist_node<K, V> *update[current_level + 1];
    memset(update, 0, sizeof(skiplist_node<K, V> *) * (current_level + 1));


    // 从上往下寻找
    for (int i = this->current_level; i >= 0; i--) {
        while (cur->forward[i] != nullptr && cur->forward[i]->key < k) {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }

    // 遍历结束后，cur指向最底层插入位置的前一个node
    // level 0包含了全部元素，如果该元素存在，那么下一个元素就是k，进行更新
    cur = cur->forward[0];
    if (cur == nullptr || cur->key != k) {
        std::cout << "key : " << k << " not exists" << std::endl;
        return 1;
    }

    // 更新0到node_level
    for (int i = 0; i <= cur->node_level; i++) {
        update[i]->forward[i] = cur->forward[i];
    }

    //删除元素
    delete cur;

    // 尝试缩小current_level
    while (current_level > 0 && header->forward[current_level] == 0) {
        --current_level;
    }

    std::cout << "successfully delete key : " << k << std::endl;
    --element_count;
    return 0;
}

template <typename K, typename V>
void skiplist<K, V>::display_list()
{
    for (int i = current_level; i >= 0; i--) {
        skiplist_node<K, V> *node = this->header->forward[i];

        while (node != nullptr) {
            std::cout << node->key << ":" << node->value << "    ";
            node = node->forward[i];
        }
        std::cout << std::endl;
    }
}

template <typename K, typename V>
int skiplist<K, V>::size()
{
    return this->element_count;
}


