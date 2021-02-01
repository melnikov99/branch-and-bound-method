#include "stdafx.h"
#include "Simplex.h"
#include <iostream>
#include <iomanip>
#include <iterator>
#include <algorithm>
#include <fstream>

using namespace std;

template <typename T>
void PrintVector(const T& v, const string& s) {
	for (auto it = v.begin(); it < v.end(); it++) {
		if (s.length() > 2 && it == v.begin())
			cout << setw(8) << fixed << setprecision(2) << *it;
		else 
			cout << setw(9) << fixed << setprecision(2) << *it;
	}	
}
vector<double> input_vector(const string& filename, int a) {
	ifstream input(filename);
	vector<double> f_vector;
	double f_element;

	while (input >> f_element)
		f_vector.push_back(a*f_element);
	return f_vector;
}
void fulling_var(vector<string>& var, const int& n, const int& m, int VarIsBase) {
	int k = m;
	if (VarIsBase == 1) {
		k = n;
	}
	for (int i = 1; i <= k; i++)
		var.push_back("x" + to_string(i + m*VarIsBase));
}
tuple<bool, int> CheckNegative(const vector<double>& v) {
	for (unsigned int i = 0; i < v.size(); i++) {
		if (v[i] < 0)
			return make_tuple(true, i);
	}
	return make_tuple(false, -1);
}
tuple<bool, int> CheckPositive(const vector<double>& v) {
	for (unsigned int i = 1; i < v.size(); i++) {
		if (v[i] > 0)
			return make_tuple(true, i);
	}
	return make_tuple(false, -1);
}
int MinPositive(const vector<double>& v1, const vector<double>& v2) {
	double min = 2000;
	int r;
	for (unsigned int i = 0; i < v1.size() - 1; i++) {
		if (v2[i] != 0 && v1[i] / v2[i] < min && v1[i] / v2[i] >= 0 && !(v1[i] >= 0 && v2[i] < 0)) {
			min = v1[i] / v2[i];
			r = i;
		}
	}
	return r;
}
vector<double> vector_from_column(const vector<vector<double>>& matr, const int& column_num) {
	vector<double> v;
	for (unsigned int i = 0; i < matr.size(); i++)
		v.push_back(matr[i][column_num]);
	return v;
}
void Jordan(vector<vector<double>>& matr, vector<string>& base, vector<string>& free, const int& r, const int& k) {
	string tmp = base[r];
	base[r] = free[k - 1];
	free[k - 1] = tmp;
	vector<vector<double>> matr_ = matr;
	for (unsigned int i = 0; i < matr.size(); i++) {
		for (unsigned int j = 0; j < matr[i].size(); j++) {
			if (i != r && j != k)
				matr_[i][j] = matr[i][j] - (matr[i][k] * matr[r][j] / matr[r][k]);
			if (i == r && j != k)
				if (matr[r][j] == 0)
					matr_[r][j] = 0;
				else
					matr_[r][j] = matr[r][j] / matr[r][k];
			if (i != r && j == k)
				if (matr[i][k] == 0)
					matr[i][k] == 0;
				else
					matr_[i][k] = (-1)*(matr[i][k] / matr[r][k]);
			if (i == r && j == k)
				matr_[r][k] = 1 / matr[r][k];
		}
	}
	matr = matr_;
}
int findk(const vector<vector<double>>& matr, const int& r_) {
	vector<double> tmp = matr[r_];
	auto iter = tmp.cbegin();
	tmp.erase(iter);
	int k = get<1>(CheckNegative(tmp));
	return k;
}
InputData::InputData() {
	b = { 0 };
	c = { 0 };
	A = { {0} };
}
InputData::InputData(const string& A1, const string& b1, const string& c1) {
	b = input_vector(b1, 1);
	c = input_vector(c1, -1);

	ifstream input(A1);
	input >> n;
	input.ignore(1);
	input >> m;
	vector<double> rows;
	double row_element;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			input >> row_element;
			rows.push_back(row_element);
		}
		A.push_back(rows);
		rows.clear();
	}
}
InputData AddCondition(const InputData& data, vector<double> a_, int b_) {
	InputData new_data = data;
	new_data.n += 1;
	new_data.A.push_back(a_);
	new_data.b.push_back(b_);
	return new_data;
}
Simplex::Simplex() {
	n = 0;
	m = 0;
	base_var = {};
	free_var = {};
	matrix = { {} };
	SolExist = true;
}
Simplex::Simplex(InputData data) {
	n = data.n;
	m = data.m;
	SolExist = true;
	vector<double> b_;
	for (int i = 0; i < n; i++) {
		b_.push_back(data.b[i]);
		for (auto item : data.A[i]) {
			b_.push_back(item);
		}
		matrix.push_back(b_);
		b_.clear();
	}
	vector<double> c_;
	c_.push_back(0);
	for (auto item : data.c)
		c_.push_back(item);
	matrix.push_back(c_);
	fulling_var(base_var, n, m, 1);
	base_var.push_back("F ");
	fulling_var(free_var, n, m, 0);
}
void Simplex::Print() {
	cout << setw(9) << "          S";
	for (unsigned int i = 0; i < free_var.size(); i++)
		cout << setw(9) << free_var[i];
	cout << endl;
	for (unsigned int i = 0; i < base_var.size(); i++) {
		cout << base_var[i];
		PrintVector(matrix[i], base_var[i]);
		cout << endl;
	}
	cout << endl;
}
void Simplex::PrintSolution(const string& str, bool SolutionExist) {
	cout << str << endl;
	if (SolutionExist) {
		for (int i = 0; i < n + 1; i++)
			cout << base_var[i] << " = " << fixed << setprecision(2) << matrix[i][0] << endl;
	}
}
tuple<bool, bool, bool> Simplex::IsReferenceSolution() {
	vector<double> s_vector;
	bool reference_flag = false;
	bool no_solution_flag = false;
	bool optimum_flag = false;

	for (int i = 0; i < n; i++)
		s_vector.push_back(matrix[i][0]);

	int r1 = get<1>(CheckNegative(s_vector));

	if (r1 < 0)
		reference_flag = true;
	else {
		int k = findk(matrix, r1);
		if (k < 0)
			no_solution_flag = true;
	}
	if (!get<0>(CheckNegative(matrix[matrix.size() - 1])))
		optimum_flag = true;
	return make_tuple(reference_flag, no_solution_flag, optimum_flag);
}
void Simplex::FindReferenceSolution() {
	vector<double> s_vector;
	for (int i = 0; i < n; i++)
		s_vector.push_back(matrix[i][0]);
	int r1 = get<1>(CheckNegative(s_vector));
	int k = findk(matrix, r1) + 1;

	vector<double> v1 = vector_from_column(matrix, 0);
	vector<double> v2 = vector_from_column(matrix, k);
	int r = MinPositive(v1, v2);

	//cout << "r = " << base_var[r] << ", k = " << free_var[k - 1] << endl;
	Jordan(matrix, base_var, free_var, r, k);
}
void Simplex::FindOptimumSolution() {
	int k = get<1>(CheckNegative(matrix[matrix.size() - 1]));
	vector<double> v1 = vector_from_column(matrix, 0);
	vector<double> v2 = vector_from_column(matrix, k);
	int r = MinPositive(v1, v2);
	//cout << "r = " << base_var[r] << ", k = " << free_var[k - 1] << endl;
	Jordan(matrix, base_var, free_var, r, k);
}
vector<string> Simplex::GetBaseVar() const {
	return base_var;
}
vector<string> Simplex::GetFreeVar() const {
	return free_var;
}
vector<double> Simplex::GetFromFirstColumn() const {
	return vector_from_column(matrix, 0);
}
bool Simplex::GetSolExist() const {
	return SolExist;
}
Simplex Solution(InputData data) {
	Simplex a(data);
	bool first_stage_flag = true;
	cout << "Start table: \n\n";
	a.Print();
	while (true) {
		//a.Print();
		if (get<0>(a.IsReferenceSolution())) {
			//a.PrintSolution("Reference solution: \n", 1);
			break;
		}
		else {
			if (get<1>(a.IsReferenceSolution())) {
				a.PrintSolution("No solution. \n", 0);
				a.SolExist = first_stage_flag = false;
				break;
			}
			else
				a.FindReferenceSolution();
		}
	}
	if (first_stage_flag) {
		int k = 7;
		//cout << "\nSecond stage: \n\n";
		while (true) {
			//a.Print();
			if (get<2>(a.IsReferenceSolution())) {
				cout << "End table: " << endl;
				a.Print();
				//a.PrintSolution("\nOptimal solution: \n", 1);
				break;
			}
			else
				a.FindOptimumSolution();
			k--;
			if (k == 0) {
				a.SolExist = false;
				return a;
			}
		}
	}
	return a;
}
