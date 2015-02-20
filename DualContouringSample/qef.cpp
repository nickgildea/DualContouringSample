#include "qef.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

namespace {

// This file is lifted from the Dual Contouring example implementation, and modified

#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);a[k][l]=h+s*(g-h*tau);

void jacobi ( const glm::mat3& u, glm::vec3& d, glm::mat3& v)
{
	int j, iq, ip, i;
	float tresh, theta, tau, t, sm, s, h, g, c, b [ 3 ], z [ 3 ];
	glm::mat3 a = u;

	for ( ip = 0; ip < 3; ip++ ) 
	{
		for ( iq = 0; iq < 3; iq++ )
		{
			v [ ip ] [ iq ] = 0.0f;
		}
		v [ ip ] [ ip ] = 1.0f;
	}

	for ( ip = 0; ip < 3; ip++ )
	{
		b [ ip ] = a [ ip ] [ ip ];
		d [ ip ] = b [ ip ];
		z [ ip ] = 0.0f;
	}

	for ( i = 1; i <= 50; i++ )
	{
		sm = 0.0f;
		for ( ip = 0; ip < 2; ip++ )
		{
			for ( iq = ip + 1; iq < 3; iq++ )
			{
				sm += (float)fabs ( a [ ip ] [ iq ] );
			}
		}

		if ( sm == 0.0f )
		{
			a = glm::transpose(v);

			if ( fabs ( d [ 0 ] ) < fabs ( d [ 1 ] ) )
			{
				sm = d [ 0 ];
				d [ 0 ] = d [ 1 ];
				d [ 1 ] = sm;

				sm = a [ 0 ] [ 0 ];
				a [ 0 ] [ 0 ] = a [ 1 ] [ 0 ];
				a [ 1 ] [ 0 ] = sm;
				sm = a [ 0 ] [ 1 ];
				a [ 0 ] [ 1 ] = a [ 1 ] [ 1 ];
				a [ 1 ] [ 1 ] = sm;
				sm = a [ 0 ] [ 2 ];
				a [ 0 ] [ 2 ] = a [ 1 ] [ 2 ];
				a [ 1 ] [ 2 ] = sm;
			}
			if ( fabs ( d [ 1 ] ) < fabs ( d [ 2 ] ) )
			{
				sm = d [ 1 ];
				d [ 1 ] = d [ 2 ];
				d [ 2 ] = sm;

				sm = a [ 1 ] [ 0 ];
				a [ 1] [ 0 ] = a [ 2 ] [ 0 ];
				a [ 2 ] [ 0 ] = sm;
				sm = a [ 1 ] [ 1 ];
				a [ 1 ] [ 1 ] = a [ 2 ] [ 1 ];
				a [ 2 ] [ 1 ] = sm;
				sm = a [ 1 ] [ 2 ];
				a [ 1 ] [ 2 ] = a [ 2 ] [ 2 ];
				a [ 2 ] [ 2 ] = sm;
			}
			if ( fabs ( d [ 0 ] ) < fabs ( d [ 1 ] ) )
			{
				sm = d [ 0 ];
				d [ 0 ] = d [ 1 ];
				d [ 1 ] = sm;

				sm = a [ 0 ] [ 0 ];
				a [ 0 ] [ 0 ] = a [ 1 ] [ 0 ];
				a [ 1 ] [ 0 ] = sm;
				sm = a [ 0 ] [ 1 ];
				a [ 0 ] [ 1 ] = a [ 1 ] [ 1 ];
				a [ 1 ] [ 1 ] = sm;
				sm = a [ 0 ] [ 2 ];
				a [ 0 ] [ 2 ] = a [ 1 ] [ 2 ];
				a [ 1 ] [ 2 ] = sm;
			}

			v = a;
			return;
		}

		if ( i < 4 )
		{
			tresh = 0.2f * sm / 9;
		}
		else
		{
			tresh = 0.0f;
		}

		for ( ip = 0; ip < 2; ip++ )
		{
			for ( iq = ip + 1; iq < 3; iq++ ) 
			{
				g = 100.0f * (float)fabs ( a [ ip ] [ iq ] );
				if ( i > 4 && (float)( fabs ( d [ ip ] ) + g ) == (float)fabs ( d [ ip ] )
					&& (float)( fabs ( d [ iq ] ) + g ) == (float)fabs ( d [ iq ] ) )
				{
					a [ ip ] [ iq ] = 0.0f;
				}
				else
				{
					if ( fabs ( a [ ip ] [ iq ] ) > tresh )
					{
						h = d [ iq ] - d [ ip ];
						if ( (float)( fabs ( h ) + g ) == (float)fabs ( h ) )
						{
							t = ( a [ ip ] [ iq ] ) / h;
						}
						else
						{
							theta = 0.5f * h / ( a [ ip ] [ iq ] );
							t = 1.0f / ( (float)fabs ( theta ) + (float)sqrt ( 1.0f + theta * theta ) );
							if ( theta < 0.0f ) 
							{
								t = -1.0f * t;
							}
						}

						c = 1.0f / (float)sqrt ( 1 + t * t );
						s = t * c;
						tau = s / ( 1.0f + c );
						h = t * a [ ip ] [ iq ];
						z [ ip ] -= h;
						z [ iq ] += h;
						d [ ip ] -= h;
						d [ iq ] += h;
						a [ ip ] [ iq ] = 0.0f;
						for ( j = 0; j <= ip - 1; j++ )
						{
							ROTATE ( a, j, ip, j, iq )
						}
						for ( j = ip + 1; j <= iq - 1; j++ )
						{
							ROTATE ( a, ip, j, j, iq )
						}
						for ( j = iq + 1; j < 3; j++ )
						{
							ROTATE ( a, ip, j, iq, j )
						}
						for ( j = 0; j < 3; j++ )
						{
							ROTATE ( v, j, ip, j, iq )
						}
					}
				}
			}
		}

		for ( ip = 0; ip < 3; ip++ )
		{
			b [ ip ] += z [ ip ];
			d [ ip ] = b [ ip ];
			z [ ip ] = 0.0f;
		}
	}
	printf ( "too many iterations in jacobi\n" );

	printf("u=\n");
	printf("%f %f %f\n", u[0].x, u[0].y, u[0].z);
	printf("%f %f %f\n", u[1].x, u[1].y, u[1].z);
	printf("%f %f %f\n", u[2].x, u[2].y, u[2].z);

	__debugbreak();
	exit ( 1 );
}

void matInverse ( glm::mat3& u, glm::vec3& w, float rvalue[][3])
{
	if ( w [ 0 ] == 0.0f )
	{
		printf ( "error: largest eigenvalue is 0!\n" );
	}
	else
	{
		for ( int i = 1; i < 3; i++ )
		{
			if ( w [ i ] < 0.001f ) // / w [ 0 ] < TOLERANCE )
			{
					w [ i ] = 0;
			}
			else
			{
				w [ i ] = 1.0f / w [ i ];
			}
		}
		w [ 0 ] = 1.0f / w [ 0 ];
	}

	rvalue [ 0 ] [ 0 ] = w [ 0 ] * u [ 0 ] [ 0 ] * u [ 0 ] [ 0 ] +
					w [ 1 ] * u [ 1 ] [ 0 ] * u [ 1 ] [ 0 ] +
					w [ 2 ] * u [ 2 ] [ 0 ] * u [ 2 ] [ 0 ];
	rvalue [ 0 ] [ 1 ] = w [ 0 ] * u [ 0 ] [ 0 ] * u [ 0 ] [ 1 ] +
					w [ 1 ] * u [ 1 ] [ 0 ] * u [ 1 ] [ 1 ] +
					w [ 2 ] * u [ 2 ] [ 0 ] * u [ 2 ] [ 1 ];
	rvalue [ 0 ] [ 2 ] = w [ 0 ] * u [ 0 ] [ 0 ] * u [ 0 ] [ 2 ] +
					w [ 1 ] * u [ 1 ] [ 0 ] * u [ 1 ] [ 2 ] +
					w [ 2 ] * u [ 2 ] [ 0 ] * u [ 2 ] [ 2 ];
	rvalue [ 1 ] [ 0 ] = w [ 0 ] * u [ 0 ] [ 1 ] * u [ 0 ] [ 0 ] +
					w [ 1 ] * u [ 1 ] [ 1 ] * u [ 1 ] [ 0 ] +
					w [ 2 ] * u [ 2 ] [ 1 ] * u [ 2 ] [ 0 ];
	rvalue [ 1 ] [ 1 ] = w [ 0 ] * u [ 0 ] [ 1 ] * u [ 0 ] [ 1 ] +
					w [ 1 ] * u [ 1 ] [ 1 ] * u [ 1 ] [ 1 ] +
					w [ 2 ] * u [ 2 ] [ 1 ] * u [ 2 ] [ 1 ];
	rvalue [ 1 ] [ 2 ] = w [ 0 ] * u [ 0 ] [ 1 ] * u [ 0 ] [ 2 ] +
					w [ 1 ] * u [ 1 ] [ 1 ] * u [ 1 ] [ 2 ] +
					w [ 2 ] * u [ 2 ] [ 1 ] * u [ 2 ] [ 2 ];
	rvalue [ 2 ] [ 0 ] = w [ 0 ] * u [ 0 ] [ 2 ] * u [ 0 ] [ 0 ] +
					w [ 1 ] * u [ 1 ] [ 2 ] * u [ 1 ] [ 0 ] +
					w [ 2 ] * u [ 2 ] [ 2 ] * u [ 2 ] [ 0 ];
	rvalue [ 2 ] [ 1 ] = w [ 0 ] * u [ 0 ] [ 2 ] * u [ 0 ] [ 1 ] +
					w [ 1 ] * u [ 1 ] [ 2 ] * u [ 1 ] [ 1 ] +
					w [ 2 ] * u [ 2 ] [ 2 ] * u [ 2 ] [ 1 ];
	rvalue [ 2 ] [ 2 ] = w [ 0 ] * u [ 0 ] [ 2 ] * u [ 0 ] [ 2 ] +
					w [ 1 ] * u [ 1 ] [ 2 ] * u [ 1 ] [ 2 ] +
					w [ 2 ] * u [ 2 ] [ 2 ] * u [ 2 ] [ 2 ];
}

float calcError ( const glm::mat3 a, const float b[], float btb, const float point[] )
{
	float rvalue = btb;
 
	rvalue += -2.0f * ( point [ 0 ] * b [ 0 ] + point [ 1 ] * b [ 1 ] + point [ 2 ] * b [ 2 ] );
	rvalue += point [ 0 ] * ( a [ 0 ] [ 0 ] * point [ 0 ] + a [ 0 ] [ 1 ] * point [ 1 ] + a [ 0 ] [ 2 ] * point [ 2 ] );
	rvalue += point [ 1 ] * ( a [ 1 ] [ 0 ] * point [ 0 ] + a [ 1 ] [ 1 ] * point [ 1 ] + a [ 1 ] [ 2 ] * point [ 2 ] );
	rvalue += point [ 2 ] * ( a [ 2 ] [ 0 ] * point [ 0 ] + a [ 2 ] [ 1 ] * point [ 1 ] + a [ 2 ] [ 2 ] * point [ 2 ] );

	return rvalue;
}

}	// namespace

glm::vec3 QEF::solve()
{
	float newB [ 3 ];
	float inv [ 3 ] [ 3 ];

	glm::mat3 A;
	A [ 0 ] [ 0 ] = AtA [ 0 ];
	A [ 0 ] [ 1 ] = AtA [ 1 ];
	A [ 0 ] [ 2 ] = AtA [ 2 ];
	A [ 1 ] [ 1 ] = AtA [ 3 ];
	A [ 1 ] [ 2 ] = AtA [ 4 ];
	A [ 1 ] [ 0 ] = AtA [ 1 ];
	A [ 2 ] [ 0 ] = AtA [ 2 ];
	A [ 2 ] [ 1 ] = AtA [ 4 ];
	A [ 2 ] [ 2 ] = AtA [ 5 ];

	glm::mat3 u;
	glm::vec3 w;
	jacobi(A, w, u);
	matInverse ( u, w, inv );

	newB [ 0 ] = AtB [ 0 ] - A [ 0 ] [ 0 ] * masspoint [ 0 ] - A [ 0 ] [ 1 ] * masspoint [ 1 ] - A [ 0 ] [ 2 ] * masspoint [ 2 ];
	newB [ 1 ] = AtB [ 1 ] - A [ 1 ] [ 0 ] * masspoint [ 0 ] - A [ 1 ] [ 1 ] * masspoint [ 1 ] - A [ 1 ] [ 2 ] * masspoint [ 2 ];
	newB [ 2 ] = AtB [ 2 ] - A [ 2 ] [ 0 ] * masspoint [ 0 ] - A [ 2 ] [ 1 ] * masspoint [ 1 ] - A [ 2 ] [ 2 ] * masspoint [ 2 ];

	glm::vec3 position;
	position [ 0 ] = inv [ 0 ] [ 0 ] * newB [ 0 ] + inv [ 1 ] [ 0 ] * newB [ 1 ] + inv [ 2 ] [ 0 ] * newB [ 2 ] + masspoint [ 0 ];
	position [ 1 ] = inv [ 0 ] [ 1 ] * newB [ 0 ] + inv [ 1 ] [ 1 ] * newB [ 1 ] + inv [ 2 ] [ 1 ] * newB [ 2 ] + masspoint [ 1 ];
	position [ 2 ] = inv [ 0 ] [ 2 ] * newB [ 0 ] + inv [ 1 ] [ 2 ] * newB [ 1 ] + inv [ 2 ] [ 2 ] * newB [ 2 ] + masspoint [ 2 ];

	error = calcError ( A, &AtB.x, BtB, &position.x );
	return position;
}

