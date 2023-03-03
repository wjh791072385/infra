#include <iostream>
#include <vector>
#include <climits>
#include <ctime>

#define RECOMMEDN_MAX_LEVEL 32

using namespace std;

template <typename V>
class skiplist_node
{
  public:
    int key;
    V value;
    vector<skiplist_node *> next;

    skiplist_node(int k, V v, int leval);

    virtual ~skiplist_node();
};

template <typename V>
skiplist_node<V>::skiplist_node(int k, V v, int level)
    : key(k), value(v), next(level, nullptr) {}

template <typename V>
skiplist_node<V>::~skiplist_node() {
    for (int i = 0; i < next.size(); i++) {
        delete next[i];
    }
}

/*******************************************************************************
*******************************************************************************/


template <typename V>
class skiplist
{
public:
    skiplist_node<V> *head;
    skiplist_node<V> *tail;

    int max_level;

public:
    skiplist(int max_level, V init_value);

    virtual ~skiplist();

    int random_level();

    skiplist_node<V> *insert(int k, V v);

    skiplist_node<V> *find(int k);

    skiplist_node<V> *delete_node(int k);

    int node_level(vector<skiplist_node<V> *> &p);

    void print_all();
};

template <typename V>
skiplist<V>::skiplist(int max_level, V init_value)
    : max_level(max_level)
{
    //初始化头尾节点
    head = new skiplist_node<V>(INT_MIN, init_value, max_level);
    tail = new skiplist_node<V>(INT_MAX, init_value, max_level);

    //每层头节点指向尾节点
    for (int i = 0; i < max_level; i++)
    {
        head->next[i] = tail;
    }
}

template <typename V>
skiplist<V>:: ~skiplist()
{
    delete head;
    delete tail;
}

template <typename V>
int skiplist<V>::random_level()
{
    int random_level = 1;
	static default_random_engine e(time(0));

    //以25%的概率增加层数，参考leveldb
	static uniform_int_distribution<int> u(0, 3);

	while (u(e) && random_level < RECOMMEDN_MAX_LEVEL)
	{
		random_level++;
	}

	return random_level;
}

template <typename V>
int skiplist<V>::node_level(vector<skiplist_node<V> *> &p)
{

}