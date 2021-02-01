#pragma once
#include <vector>
#include <string>
#include <tuple>

using namespace std;
struct InputData {
	InputData();
	InputData(const string& A1, const string& b1, const string& c1);

	int n;
	int m;
	vector<double> b;
	vector<double> c;
	vector<vector<double>> A;
};
InputData AddCondition(const InputData& data, vector<double> a_, int b_);

template <typename T>
void PrintVector(const T& v, const string& s);

vector<double> input_vector(const string& filename, int a);

void fulling_var(vector<string>& var, const int& n, const int& m, int VarIsFree);

tuple<bool, int> CheckNegative(const vector<double>& v);

tuple<bool, int> CheckPositive(const vector<double>& v);

int MinPositive(const vector<double>& v1, const vector<double>& v2);

vector<double> vector_from_column(const vector<vector<double>>& matr, const int& column_num);

void Jordan(vector<vector<double>>& matr, vector<string>& base, vector<string>& free, const int& r, const int& k);

int findk(const vector<vector<double>>& matr, const int& r_);

class Simplex {
public:
	Simplex();
	Simplex(InputData data);
	
	void Print();
	void PrintSolution(const string& str, bool SolutionExist);
	tuple<bool, bool, bool> IsReferenceSolution();

	void FindReferenceSolution();
	void FindOptimumSolution();
	
	vector<string> GetBaseVar() const;
	vector<string> GetFreeVar() const;
	vector<double> GetFromFirstColumn() const;
	bool GetSolExist() const;

	friend Simplex Solution(InputData data);
private:
	int n;
	int m;
	vector<string> base_var;
	vector<string> free_var;
	vector<vector<double>> matrix;
	bool SolExist;
};
