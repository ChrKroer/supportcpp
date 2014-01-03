

class Probability {
public:
    static unsigned nChoosek(unsigned n, unsigned k, double multiplier);
    static double CDFGaussian(double mean, double std_dev, double a);
    static double GaussianProbabilityOfInterval(double mean, double std_dev, double a, double b);
};
