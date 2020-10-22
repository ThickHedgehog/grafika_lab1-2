#pragma once
void CalcNormal(double x1[3], double x2[3], double x3[3], double* n)
{
	double p1[3], p2[3];
	p1[0] = x2[0] - x1[0];
	p1[1] = x2[1] - x1[1];
	p1[2] = x2[2] - x1[2];

	p2[0] = x3[0] - x1[0];
	p2[1] = x3[1] - x1[1];
	p2[2] = x3[2] - x1[2];

	n[0] = p1[1] * p2[2] - p2[1] * p1[2];
	n[1] = (-1) * p1[0] * p2[2] + p2[0] * p1[2];
	n[2] = p1[0] * p2[1] - p2[0] * p1[1];
}