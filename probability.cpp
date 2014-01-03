#include <random>

#include "probability.h"

unsigned Probability::nChoosek(unsigned n, unsigned k, double multiplier) {
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    int result = n;
    for( int i = 2; i <= k; ++i ) {
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

double Probability::CDFGaussian(double mean, double std_dev, double a){
    return 0.5 * (1 + erf((a - mean) / (std_dev * sqrt(2))));
}

double Probability::GaussianProbabilityOfInterval(double mean, double std_dev, double a, double b){
    return 0.5 * (erf((b - mean) / (std_dev * sqrt(2))) - erf((a - mean) / (std_dev * sqrt(2))));
}
