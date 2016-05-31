clear;
clf;

qbase = 5;
nbr_bands = 6;
fl = 400;
fh = 1600;
w = logspace (log10 (1), log10 (20000), 10001);

for band = 0 : nbr_bands-1

	f  = fl * ((fh/fl) ^ (band / (nbr_bands-1)));
	ff = [1/(f*f) 1/f 1];

	q = qbase * (1000 / f);
	g = ((f - 100) / f)^2;

	b = [0.5/q qbase/q 0.05];
	a = [1 g/q 1];

	b = b .* ff;
	a = a .* ff;

	h = freqs (b, a, w);
	hh = 20 * log10 (abs (h));
	semilogx (w, hh);
	hold on;

end;

axis ([1 20000 -40 30]);
grid on;
hold off;
