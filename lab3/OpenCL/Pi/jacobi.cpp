#include <stdio.h>
#include <stdlib.h>

#include "jacobi.h"
#include "jacobian.h"


void jacobistep(double *psinew, double *psi, int m, int n)
{
	/*for (i = 1;i <= m;i++) {
		for(j=1;j<=n;j++) {
			psinew[i * (m + 2) + j] = 0.25 * (psi[(i - 1) * (m + 2) + j] + psi[(i + 1) * (m + 2) + j] + psi[i * (m + 2) + j - 1] + psi[i * (m + 2) + j + 1]);
		}
	}*/
	calc_jacobian(psinew, psi, m, n);
}


double deltasq(double *newarr, double *oldarr, int m, int n)
{
	int i, j;

	double dsq=0.0;
	double tmp;

	// MOŽE SE PARALELIZIRATI, ALI SE NE ISPLATI JER SE IZVODI SAMO NA KRAJU

	for (i = 1;i <= m;i++)
	{
		for(j=1;j<=n;j++)
	{
		tmp = newarr[i*(m+2)+j]-oldarr[i*(m+2)+j];
		dsq += tmp*tmp;
		}
	}

	/*double dsqp = delta_squared(newarr + 1, oldarr + 1, m * n);
	if (dsqp != dsq) {
		printf("Razlicite vrijednosti za dsq: %f %f\n", dsq, dsqp);
	}*/

	return dsq;
}
