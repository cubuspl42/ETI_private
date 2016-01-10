extern "C" {
	void szybki_max(double * t1, double * t2, double * w, int n);
}

const unsigned n = 8;

int main()
{
	double t1[n] = { 1.0,2.0,2.6,3.0,1.0,2.3,4.5,8.7 };

	double t2[n] = { 2.0,1.0,2.5,3.2,1.8,2.2,4.1,8.2 };

	double w[n];

	szybki_max(t1, t2, w, 8);

	return 0;
}