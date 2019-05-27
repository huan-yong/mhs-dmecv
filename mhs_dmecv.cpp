#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/time.h>
#include <vector>
using namespace std;

class Node {
public:
	int i;
	int count;
public:
	Node(int i, int count);
};

Node::Node(int i, int count) {
	this->i = i;
	this->count = count;
}

class SetNode {
public:
	int setNum;
	SetNode* next;
public:
	SetNode(int setNum, SetNode* next);
};

SetNode::SetNode(int setNum, SetNode* next) {
	this->setNum = setNum;
	this->next = next;
}

class ListNode {
public:
	SetNode* adjacent;
public:	
	ListNode(SetNode* adjacent = NULL);
    ~ListNode() {
        while (adjacent) {
            SetNode* temp = adjacent->next;
            delete adjacent;
            adjacent = temp;
        }
    }
};

ListNode::ListNode(SetNode* adjacent) {
	this->adjacent = adjacent;
}

class HittingSet {
public:
	int* flag; //the set cover flag
	ListNode* head; //hash table structure
	vector<Node*> calculate; //use to compute minimal hitting set	
	vector<int> hittingSet; //store minimal hitting set
	int alreadyCover; //already cover count
	int m, n; //m: element count, n: set count
	int sum; //minimal hitting set count
    int hsSum;
public:
	HittingSet();
    ~HittingSet() {
        if (flag)
            delete[] flag;
        if (head)
            delete[] head;
        vector<Node*>().swap(calculate);
        vector<int>().swap(hittingSet);
    }
	void getMinHittingSet(vector<Node*> a, ListNode* h2);
	bool judgeMinHittingSet(vector<int> hs);
	void init();
private:
	void recover(SetNode* node);
	void insertSort(vector<Node*>* t);
	void insert(ListNode* listNode, SetNode* setNode);
};


HittingSet::HittingSet() {
	flag = NULL;
    head = NULL;
	alreadyCover = 0;
	m = n = 0;
	sum = 0;
    hsSum = 0;
}

void HittingSet::init() {
	alreadyCover = 0;
	sum = 0;
	flag = new int[n];
    for (int i = 0; i < n; i++)
        flag[i] = 0;
	for (int i = 0; i < m; i++) {
		SetNode* temp = head[i].adjacent;
		int t = 0;
		while (temp) {
			t++;
			temp = temp->next;
		}
		if (t != 0)
			calculate.push_back(new Node(i, t));
	}
	insertSort(&calculate);
}

void input(char fileName[]) {
    HittingSet hs = HittingSet();
    vector<vector<int> > list;
    int element = 0;
    string line2;
    ifstream ifile(fileName);
    if (!ifile) {
        printf("Open file %s error.\n", fileName);
        exit(1);
    }
    while (getline(ifile, line2)) {
        vector<int> vec;
        int temp = 0;
        for (int i = 0; ; i++) {
            char c = line2[i];
            if (c == '\0') {
                if (temp > element)
                    element = temp;
                vec.push_back(temp);
                break;
            } else if (c >= '0' && c <= '9')
                temp = temp * 10 + c - '0';
            else if (c == ' ') {
                if (temp > element)
                    element = temp;
                vec.push_back(temp);
                temp = 0;
            }
        }
        list.push_back(vec);
    }

    hs.m = element;
    hs.n = list.size(); 
    int m = hs.m, n = hs.n;
    
    hs.head = new ListNode[m];
    ListNode* head = hs.head;
    for (int i = 0; i < m; i++)
        head[i].adjacent = NULL;
    for (int i = 0; i < n; i++) {
        vector<int> a = list[i];
        for (int j = 0; j < a.size(); j++) {
            int t = a[j] - 1;
            SetNode* set = head[t].adjacent;
            SetNode* node = new SetNode(i, NULL);
            if (!set)
                head[t].adjacent = node;
            else {
                while (set->next)
                    set = set->next;
                set->next = node;
            }
        }
    }

    for (int i = 0; i < list.size(); i++)
        vector<int>().swap(list[i]);
    vector<vector<int> >().swap(list);
    hs.init();
    struct timeval start, end;
    gettimeofday(&start, NULL);
    hs.getMinHittingSet(hs.calculate, hs.head);
    gettimeofday(&end, NULL);
    long long time_use = 1e6 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    printf("%s\t%d\t%fs\t%lldus\n", fileName, hs.sum, time_use / (1e6 + 0.0), time_use);
    //printf("%d\n", hs.hsSum);
    ifile.close();
    ifile.clear();
}

bool HittingSet::judgeMinHittingSet(vector<int> hs) {
	int* f = new int[n];
    for (int i = 0; i < n; i++)
        f[i] = 0;
	int size = hs.size();
	for (int i = 0; i < size; i++) {
		int t = hs[i];
		SetNode* temp = head[t].adjacent;
		while (temp) {
			f[temp->setNum]++;
			temp = temp->next;
		}
	}

	for (int i = 0; i < size; i++) {
		int t = hs[i];
		SetNode* temp = head[t].adjacent;
		while (temp) {
			f[temp->setNum]--;
			temp = temp->next;
		}
		bool f2 = false;
		for (int j = 0; j < n; j++) {
			if (f[j] == 0) {
				f2 = true;
				break;
			}
		}
		if (!f2) {
            delete[] f;
			return false;
        }
		temp = head[t].adjacent;
		while (temp) {
			f[temp->setNum]++;
			temp = temp->next;
		}
	}
    delete[] f;
	return true;
}

void HittingSet::insert(ListNode* listNode, SetNode* setNode) {
	if (!listNode || !setNode)
		return;
	if (!(listNode->adjacent))
		listNode->adjacent = setNode;
	else {
		SetNode* t = listNode->adjacent;
		listNode->adjacent = setNode;
		setNode->next = t;
	}
}

void HittingSet::insertSort(vector<Node*>* t) {
	if (!t || (*t).size() < 2)
		return;
	int i = 1;
	int size = (*t).size();
	while (i < size) {
		Node* temp = (*t)[i];
		int j = i - 1;
		while (j >= 0 && (*t)[j]->count < temp->count) {
			(*t)[j + 1] = (*t)[j];
			j--;
		}
		(*t)[j + 1] = temp;
		i++;
	}
}

void HittingSet::getMinHittingSet(vector<Node*> a, ListNode* h2) {
	for (int i = 0; i < a.size(); i++) {
		Node* temp = a[i];
		if (i == a.size() - 1 && temp->count + alreadyCover < n)
			return;
		if (temp->count + alreadyCover == n) {
            hsSum++;
			hittingSet.push_back(temp->i);
			if (judgeMinHittingSet(hittingSet)) {
				sum++;
                /*
				int size = hittingSet.size();
				for (int j = 0; j < size; j++) {
					if (j == size - 1)
						printf("%d\n", hittingSet[j] + 1);
					else
						printf("%d ", hittingSet[j] + 1);
				}
                */
			}
			hittingSet.pop_back();
			continue;
		}
		hittingSet.push_back(temp->i);
		SetNode* temp2 = h2[temp->i].adjacent;
		while (temp2) {
			int set = temp2->setNum;
			if (flag[set] == 0)
				alreadyCover++; //already cover add 1
			flag[set]++;
			temp2 = temp2->next;
		}

		bool* fs = new bool[n];
		for (int j = 0; j < n; j++)
			fs[j] = false;
		vector<Node*> t;
		ListNode* h = new ListNode[m];
		for (int j = 0; j < m; j++)
			h[j].adjacent = NULL;
		int canCover = 0;
		for (int j = i + 1; j < a.size(); j++) {
			Node* x = a[j];
			SetNode* temp3 = h2[x->i].adjacent;
			int t2 = 0;
			while (temp3) {
				if (flag[temp3->setNum] == 0) {
					insert(&h[x->i], new SetNode(temp3->setNum, NULL));
					t2++;
					if (!fs[temp3->setNum]) {
						canCover++;
						fs[temp3->setNum] = true;
					}
				}
				temp3 = temp3->next;
			}
			if (t2 != 0)
				t.push_back(new Node(x->i, t2));
		}
			
		if (alreadyCover + canCover < n) {
			hittingSet.pop_back();
			recover(h2[temp->i].adjacent);
			return;
		}

		insertSort(&t);
		getMinHittingSet(t, h);
		hittingSet.pop_back();
		recover(h2[temp->i].adjacent);

        vector<Node*>().swap(t);
        delete[] h;
        delete[] fs;
	}
}

void HittingSet::recover(SetNode* node) {
	while (node) {
		int set = node->setNum;
		flag[set]--;
		if (flag[set] == 0)
			alreadyCover--;
		node = node->next;
	}
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Argument count %d error\n", argc);
        return 1;
    }
	input(argv[1]);
	return 0;
}
