/*
 * $File: main.cc
 * $Date: Wed Dec 11 12:28:33 2013 +0800
 * $Author: Xinyu Zhou <zxytim[at]gmail[dot]com>
 */

#include <cstdio>
#include <fstream>

#include "gmm.hh"

#include "datamanip.hh"
#include "common.hh"

#include "tclap/CmdLine.h"

using namespace std;
using namespace TCLAP;

typedef std::vector<std::vector<real_t>> DenseDataset;

vector<real_t> string_to_double_vector(string line) {
	vector<real_t> x;
	int begin = 0, end = 0;
	int len = line.size();
	while (true) {
		while (end < len && line[end] != ' ' && line[end] != '\n')
			end ++;
		x.push_back(atof(line.substr(begin, end - begin).c_str()));
		if (end == len - 1 || line[end] == '\n' || (end == len - 2 && line[end] == ' ' && line[end] == '\n'))
			break;
		begin = end + 1;
		end = begin;
	}
	return x;
}

struct Args {
	int concurrency;
	int K;
	int iteration;
	real_t min_covar = 1e-3;

	string input_file;
	string model_file;
};

Args parse_args(int argc, char *argv[]) {
	Args args;
	try {
		CmdLine cmd("Gaussian Mixture Model (GMM)", ' ', "0.0.1");

		ValueArg<int> arg_concurrency("w", "concurrency", "number of workers", false, 1, "NUMBER");
		ValueArg<int> arg_K("k", "K", "number of gaussians", true, 10, "NUMBER");
		ValueArg<double> arg_min_covar("c", "mincovar", "minimum covariance to avoid overfitting, default 1e-3.", false, 1e-3, "FLOAT");

		ValueArg<string> arg_input_file("i", "input", "intput file", true, "", "FILE");
		ValueArg<string> arg_model_file("m", "model", "model file", true, "", "FILE");
		ValueArg<int> arg_iteration("r", "iteration", "number of iterations",
				false, 200, "NUMBER");

		cmd.add(arg_concurrency);
		cmd.add(arg_K);
		cmd.add(arg_min_covar);
		cmd.add(arg_input_file);
		cmd.add(arg_model_file);
		cmd.add(arg_iteration);


		cmd.parse(argc, argv);

#define GET_VALUE(name) args.name = arg_##name.getValue();
		GET_VALUE(concurrency);
		GET_VALUE(K);
		GET_VALUE(min_covar);
		GET_VALUE(input_file);
		GET_VALUE(model_file);
		GET_VALUE(iteration);

	} catch (ArgException &e) {
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
	}
	return args;
}

void read_dense_dataset(DenseDataset &X, const char *fname) {
	ifstream fin(fname);
	string line;
	while (getline(fin, line)) {
		X.push_back(string_to_double_vector(line));
	}
}

void write_dense_dataset(DenseDataset &X, const char *fname) {
	ofstream fout(fname);
	for (auto &x: X) {
		for (auto &v: x)
			fout << v << ' ';
		fout << endl;
	}
}

int main(int argc, char *argv[]) {
	GMM test("python/gmm.model");
	return 0;
//    srand(42); // Answer to The Ultimate Question of Life, the Universe, and Everything
	Args args = parse_args(argc, argv);

	DenseDataset X;
	read_dense_dataset(X, args.input_file.c_str());

	GMMTrainerBaseline trainer(args.iteration, args.min_covar, args.concurrency);
	GMM gmm(args.K, COVTYPE_DIAGONAL, &trainer);
	printf("start training ...\n"); fflush(stdout);
	gmm.fit(X);

	ofstream fout(args.model_file);
	gmm.dump(fout);

	return 0;
}

/**
 * vim: syntax=cpp11 foldmethod=marker
 */

