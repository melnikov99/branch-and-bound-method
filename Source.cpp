// ILP.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include<iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "Simplex.h"
using namespace std;
using   Answer = map<string, double>;
using	Conditions = pair<vector<double>, int>;

int ParseString(const string& s) {
	int num = 0;
	stringstream ss(s);
	ss.ignore(1);
	ss >> num;
	return num;
}
map<string, double> getX(const Simplex& p) {
	map<string, double> solution;
	auto matrixS = p.GetFromFirstColumn();
	auto var = p.GetBaseVar();
	auto freevar = p.GetFreeVar();
	int num;
	for (unsigned int i = 1; i < freevar.size() + 1; i++)
		solution["x" + to_string(i)] = 0;
	for (unsigned int i = 0; i < var.size() - 1; i++) {
		num = ParseString(var[i]);
		switch (num) {
			case 1: {
				solution["x1"] = matrixS[i];
				break;
			}
			case 2: {
				solution["x2"] = matrixS[i];
				break;
			}
			case 3: {
				solution["x3"] = matrixS[i];
				break;
			}
			default: {
				break;
			}
		}
	}
	solution["F "] = matrixS[matrixS.size() - 1];
	return solution;
}  
bool IsInteger(double a) {
	return (a - floor(a) == 0);
}
bool IsSolutionInteger(const Answer& ans) {
	bool flag = true;
	for (const auto& item : ans)
		flag = flag && IsInteger(item.second);
	return flag;
}
string FindFirstDoubleIndex(const map<string, double>& m) {
	for (const auto& item : m) {
		if (!IsInteger(item.second) && item.first != "F ")
			return item.first;
	}
	string empty_s("");
	return empty_s;
}
struct Tnode {
	Tnode();
	InputData data;
	Answer answer;
	Tnode *l;
	Tnode *r;
};
Tnode::Tnode() {
		l = NULL;
		r = NULL;
		answer = { {"", 0} };
}

void add(InputData x, Tnode *&t, bool isleft) {
	if (t == NULL) {
		t = new Tnode;
		t->data = x;
		t->l = NULL;
		t->r = NULL;
		auto sol = Solution(x);
		if (sol.GetSolExist()) {
			t->answer = getX(sol);
		}
		else
			t->answer = { {"empty solution", 404} };
	}
	else
		if (isleft)
			add(x, t->l, isleft);
		else
			add(x, t->r, isleft);
}
vector<Tnode*> leafs;
vector<Tnode*> GetAllLeafs(Tnode *t) {
	if (t == NULL)
		return leafs;
	if (t->l == NULL && t->r == NULL && t->answer.size() != 1) {
		leafs.push_back(t);
	}
	else {
		if (t->l == NULL && t->r != NULL) {
			GetAllLeafs(t->r);
		}
		else {
			if (t->l != NULL && t->r == NULL) {
				GetAllLeafs(t->l);
			}
			else {
				GetAllLeafs(t->l);
				GetAllLeafs(t->r);
			}
		}
	}
	return leafs;
}
void Delete(Tnode *t) {
	if (t != NULL) {
		if (t->l == NULL && t->r == NULL)
			delete t;
		else {
			Delete(t->l);
			Delete(t->r);
		}
	}
}
bool CheckAllLeafs(const vector<Tnode*> v, int record) {
	bool flag = true;
	bool record_flag = true;
	for (const auto item : v) {
		flag = flag && IsSolutionInteger(item->answer);
		record_flag = record_flag && (item->answer.at("F ") <= record);
	}
	return flag || record_flag;
}
Tnode* MaxLeaf(const vector<Tnode*>& v) {
	if (v.empty())
		return NULL;
	double max_ = -1;
	Tnode* max_d = v[0];
	for (const auto& item : v) {
		if (!IsSolutionInteger(item->answer)) {
			if (item->answer.at("F ") >= max_) {
				max_ = item->answer.at("F ");
				max_d = item;
			}
		}
	}
	return max_d;
}
int GetRecord(const vector<Tnode*>& v) {
	int rec = -1;
	for (const auto& item : v) {
		if (IsSolutionInteger(item->answer) && item->answer.at("F ") > rec)
			rec = item->answer.at("F ");
	}
	return rec;
}
pair<Conditions, Conditions> CreateConditionPair(const Answer& ans) {
	string var_name = FindFirstDoubleIndex(ans);
	int var_index = ParseString(var_name);
	
	int x_less = 0, x_more = 0;
	vector<double> v_less, v_more;

	if (var_name != "") {
		x_less = int(floor(ans.at(var_name)));
		x_more = int(-x_less - 1);
		for (int i = 0; i < 3; i++) {
			if (var_index - 1 == i) {
				v_more.push_back(-1);
				v_less.push_back(1);
			}
			else {
				v_more.push_back(0);
				v_less.push_back(0);
			}
		}
	}
	return make_pair(make_pair(v_less, x_less), make_pair(v_more, x_more));
}
template <typename T>
T FValue(const vector<T>& v, const vector<T>& d) {
	return v[0] * d[0] + v[1] * d[1] + v[2] * d[2];
}
template <typename T> 
bool CheckConditions(const vector<T>& v, const InputData& d) {
	bool flag = true;
	for (unsigned int i = 0; i < d.A.size(); i++) {
		flag = flag && (FValue(v, d.A[i]) <= d.b[i]);
	}
	return flag;
}
template<typename First, typename Second>
ostream& operator << (ostream& out, const pair<First, Second>& p) {
	cout << p.first << " = " << setprecision(2) << p.second << endl;
	return out;
}
template<typename Key, typename Value>
ostream& operator << (ostream& out, const map<Key, Value>& m) {
	for (const auto item : m)
		cout << item;
	return out;
}
template<typename T>
ostream& operator << (ostream& out, const vector<T>& v) {
	for (auto it = v.begin(); it != v.end(); it++) {
		if (it == v.begin())
			cout << "[ ";
		cout << *it;
		if (it == v.end() - 1)
			cout << " ]";
		else
			cout << ", ";
	}
	return out;
}
int main()
{
	const InputData data("A.txt", "b.txt", "c.txt");
	cout << "enter 1 to use branch and bound method. \nenter 2 to use bruteforce.\n";
	int q;
	cin >> q;
	switch (q) {
	case 1: {
		Tnode *d = NULL;
		add(data, d, true);
		Tnode *d1 = d;
		int i = 1;
		int record = 0;
		while (!CheckAllLeafs(GetAllLeafs(d1), record)) {
			cout << "TASK " << i << endl;
			leafs.clear();
			auto cndtn = CreateConditionPair(d->answer);
			auto dl = AddCondition(d->data, cndtn.first.first, cndtn.first.second);
			auto dr = AddCondition(d->data, cndtn.second.first, cndtn.second.second);
			cout << "LEFT TASK " << i << endl;
			add(dl, d, true);
			cout << fixed << setprecision(2) << d->l->answer << endl;
			cout << "RIGHT TASK " << i << endl;
			add(dr, d, false);
			cout << fixed << setprecision(2) << d->r->answer << endl;
			auto v = GetAllLeafs(d1);
			auto m = MaxLeaf(v);
			record = GetRecord(v);
			cout << record << endl;
			d = m;
			i++;
		}
		leafs.clear();
		auto l = GetAllLeafs(d1);
		sort(l.rbegin(), l.rend(),
			[](Tnode *lhs, Tnode *rhs) {
			return lhs->answer.at("F ") < rhs->answer.at("F ");
		});
		cout << "Final Integer Solution: \n\n" << l[0]->answer;
		Delete(d);
		break;
	}
	case 2: {
		map<vector<double>, int> m;
		vector<double> v;
		int bs = 0;
		vector<double> bv;
		for (const int i1 : { 6, 5, 4, 3, 2, 1, 0 }) {
			for (const int i2 : { 2, 1, 0 }) {
				v.push_back(i1);
				v.push_back(i2);
				v.push_back(0);
				if (CheckConditions(v, data)) {
					m[v] = abs(FValue(v, data.c));
					if (m[v] > bs) {
						bs = m[v];
						bv = v;
					}
					cout << v << " F = " << m[v] << endl;
				}
				v.clear();
			}
		}
		cout << "Best integer solution is: x = " << bv << " F = " << bs << endl;
		break;
	}
	default: 
		break;
	}

	return 0;
}
