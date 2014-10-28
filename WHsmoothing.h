#ifndef WHSMOOTHING_H
#define WHSMOOTHING_H

#include <iostream>
#include <vector>
#include <TMath.h>

void smooth_f(std::vector<float> Y, float sig, int CA, std::vector<float> & X, float & score){


	int z = CA * 1024;
	const int n = 1024;
	int nc = (n + 1) / 2;
	double e[n - 1] = {0.};
	double f[n] = {0.};
	double x[n] = {0.};
	double lam = 4 * pow(sig, 4) / (1 - sig * sig);
	double a1 = 1 + lam, a2 = 5 + lam, a3 = 6 + lam;

	double d = a1;
	f[0] = 1./d;
	x[0] = f[0] * lam * Y.at(0+z);
	double mu = 2.;
	e[0] = mu * f[0];

	d = a2 - mu * e[0];
	f[1] = 1./d;
	x[1] = f[1] * (lam * Y.at(1+z) + mu * x[0]);
	mu = 4 - e[0];
	e[1] = mu * f[1];

	int m1, m2;
	for(int j = 2; j < n-2; j++){

		m1 = j - 1;
		m2 = j - 2;
		d = a3 - mu * e[m1] - f[m2];
		f[j] = 1./d;
		x[j] = f[j] * (lam * Y.at(j+z) + mu * x[m1] - x[m2]);
		mu = 4 - e[m1];
		e[j] = mu * f[j];
	}

	d = a2 - mu * e[n-3] - f[n-4];
	f[n-2] = 1./d;
	x[n-2] = f[n-2] * (lam * Y.at(n-2+z) + mu * x[n-3] - x[n-4]);
	mu = 2 - e[n-3];
	e[n-2] = mu * f[n-2];

	d = a1 - mu * e[n-2] - f[n-3];
	f[n-1] = 1./d;
	x[n-1] = f[n-1] * (lam * Y.at(n-1+z) + mu * x[n-2] - x[n-3]);

	double sq = (Y.at(n-1+z) - x[n-1]) * (Y.at(n-1+z) - x[n-1]);
	x[n-2] = x[n-2] + e[n-2] * x[n-1];
	sq += (Y.at(n-2+z) - x[n-2]) * (Y.at(n-2+z) - x[n-2]);

	for (int j = n-3; j > 1; j--){

		x[j] = x[j] + e[j] * x[j+1] - f[j] * x[j+2];
		sq += (Y.at(j+z) - x[j]) * (Y.at(j+z) - x[j]);
	}



sq = sq / n;

X.clear();

for (int j = 0; j < n; j ++){
	X.push_back(x[j]);
}


//GCV score

double g1, g2, tr, h, q;

g2 = f[n-1];
tr = g2;
h = e[n-2] * g2;
g1 = f[n-2] + e[n-2] * h;
tr += g1;

for (int k = n-3; k > n - nc - 1; k--){

	q = e[k] * h - f[k] * g2;
	h = e[k] * g1 - f[k] * h;
	g2 = g1;
	g1 = f[k] + e[k] * h - f[k] * q;
	tr += g1;
}

tr = (2 * tr - n%2 * g1) * lam / n;
score = sq / ((1 - tr) * ( 1- tr));

return;

}


std::vector<float> WHsmoothBest_f(std::vector<float> Y, int CA=0){
//WH smoothing parameter: 'sigma'
//WH smoothing returns so called 'GCV score'
//GCV score has to be minimized
//minimize GCV score by variation of sigma:
	//start with 4 sigma test values
	//get GCV score for each sigma
	//get sigma with lowest GCV score (=sigma[min])
	//get GCV score somewhere between sigma[min] and sigma [min+-1]
	//see if it is lower
	//...
	//do so until distance delta between neighboring sigma test values is < 0.01 (empirical value)

	std::vector<float> X;
	float score = 0.;
	float sig = 0.;
	float sigma[4] = {0.01, 0.33, 0.66, 0.99}; // sigma test values
	float sig1, sig2 = 0.;
	float value[4] = {0.};
	float val = 0.;
	int min = 0;
	for (int i = 0; i < 4; i++){
		smooth_f(Y, sigma[i], CA, X, score);
		value[i] = score;

		if (i>0) {
			if (value[i] < value[i-1]){
				min = i;
				val = value[i];
			}
		}
	}
	if (min == 0 || min == 3) {
//		cout << "Couldn't find minimum score value." << endl;
		return X;
	}
	float delta = sigma[min+1]-sigma[min-1];
	while (delta > 0.01){

		sig1 = (sigma[min-1] + sigma[min]) / 2;
		smooth_f(Y, sig1, CA, X, score);
//		cout << "sig1 :" << sig1 << " score sig1: " << score << endl;
		if (score < val) {
			delta = sigma[min]-sigma[min-1];
			val = score;
			sigma[1] = sigma[min-1];
			sigma[2] = sig1;
			sigma[3] = sigma[min];
		}
		else {
			sig2 = (sigma[min] + sigma[min+1]) / 2;
			smooth_f(Y, sig2, CA, X, score);
//			cout << "sig2 :" << sig2 << " score sig2: " << score << endl;
			if (score < val){
				delta = sigma[min+1]-sigma[min];
				val = score;
				sigma[1] = sigma[min];
				sigma[2] = sig2;
				sigma[3] = sigma[min+1];
			}
			else{
//				cout << "special case" << endl;
				delta = sig2 - sig1;
				sigma[1] = sig1;
				sigma[2] = sigma[min];
				sigma[3] = sig2;

			}
		}
		min = 2;
	}

	smooth_f(Y, sigma[2], CA, X, score);
	sig = sigma[2];
	return X;

}



#endif
