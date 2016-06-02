
% Boss DS-1 tone stage frequency response
% http://www.electrosmash.com/boss-ds1-analysis
% p = tone parameter (p = 0 -> low boost, p = 1 -> high boost)
% m = additional mid boost

% ia, ib and ic are loop currents
%(ia - ib) * (1/(s*C11) + R15) + (ia - ic) * R17 - Ve = 0
%ib * R16 + (ib - ic) * R3 + (ib - ia) * (1/(s*C11) + R15) = 0
%ic * (1/(s*C12)) + (ic - ia) * R17 + (ic - ib) * R3 = 0
%
%Vs = ia * R17 + (ib - ic) * p * R3
%
%
%sln = FullSimplify [
%  Solve[{(ia - ib)*(1/(s*C11) + R15) + (ia - ic)*R17 - Ve == 0, 
%    ib*R16 + (ib - ic)*R3 + (ib - ia)*(1/(s*C11) + R15) == 0, 
%    ic*(1/(s*C12)) + (ic - ia)*R17 + (ic - ib)*R3 == 0}, {ia, ib, 
%    ic}]]
%
%sia = (ia /. sln)[[1]]
%sib = (ib /. sln)[[1]]
%sic = (ic /. sln)[[1]]
%
%FullSimplify[((sia-sic)*R17 + (sib - sic)*p*R3)/Ve]
%
%Result:
%(R17 + p * R3    +    (C11 * R17 * (R15 + R16 + R3) + p * R3 * C11 * R15 ) * s    +    C11 * C12 * (1 - p) * R16 * R17 * R3 * s^2)
%/ (R16 + R17 + R3    +   (C12*R16*(R17 + R3) + C11*(R17*(R16 + R3) + R15*(R16 + R17 + R3)))*s    +    C11*C12*R16*(R17*R3 + R15*(R17 + R3))*s^2)
%
% p is inverted

clear;
clf;

nbr_curves = 11;
w = logspace (log10 (1), log10 (20000), 10001);

for curve = 0 : nbr_curves-1

	C11 =  22e-9;
	C12 = 100e-9;
	R3  = 20e3;
	R15 = 2200;
	R16 = 6800;
	R17 = 6800;

	% Neutral: p = 0.62, m = 0.4
	% Original: m = 0

	p = curve / (nbr_curves - 1);
	m = 0.0;

%	p = 0.62;
%	m = curve / (nbr_curves - 1);

	b0 = R17 + (1 - p) * R3;
	b1 = C11 * R17 * (R15 + R16 + R3) * (1 + m * 2) + (1 - p) * R3 * C11 * R15;
	b2 = C11 * C12 * R16 * R17 * R3 * p;
	a0 = R16 + R17 + R3;
	a1 = (C12 * R16 * (R17 + R3) + C11 * (R17 * (R16 + R3) + R15 * (R16 + R17 + R3))) * (1 - m * 0.5);
	a2 = C11 * C12 * R16 * (R17 * R3 + R15 * (R17 + R3));

	bs = [b2 b1 b0];
	as = [a2 a1 a0];

	h = freqs (bs, as, w * 2 * pi);
	semilogx (w, 20*log10(abs(h)));
	hold on;

end;

hold off;
grid on;
