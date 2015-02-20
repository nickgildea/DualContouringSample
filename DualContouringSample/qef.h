#ifndef	QEF_H_HAS_BEEN_INCLUDED
#define	QEF_H_HAS_BEEN_INCLUDED

#include <glm/glm.hpp>

struct QEF
{
	QEF()
	{
		clear();
	}

	void clear()
	{
		for (int i = 0; i < 6; i++)
		{
			AtA[i] = 0.f;
		}

		AtB = glm::vec3(0.f);
		BtB = 0.f;
		masspoint = glm::vec3(0.f);
		error = 0.f;
	}

	// construct the QR representation discussed in the paper using normals at the 
	// zero-crossing and the world space position of the zero-crossing 
	void initialise(int count, const glm::vec3 normals[], const glm::vec3 positions[])
	{
		for (int i = 0; i < count; i++)
		{
			const glm::vec3& n = normals[i];
			const glm::vec3& p = positions[i];

			AtA[0] += n.x * n.x;
			AtA[1] += n.x * n.y;
			AtA[2] += n.x * n.z;
			AtA[3] += n.y * n.y;
			AtA[4] += n.y * n.z;
			AtA[5] += n.z * n.z;

			const float pn = glm::dot(p, n);
			AtB += n * pn;
			BtB += pn * pn;
			masspoint += p;
		}

		if (count)
		{
			masspoint /= (float)count;
		}
	}

	// note that even with add operation the masspoint will need to be a sum and need to be
	// divided so that it is actually the masspoint (this is a bit clunky)
	void add(const QEF& other)
	{
		for (int j = 0; j < 6; j++)
		{
			AtA[j] += other.AtA[j];
		}

		AtB += other.AtB;
		BtB += other.BtB;
		masspoint += other.masspoint;
		error += other.error;
	}

	// wrapper around the "calcPoint" function in the original code, optionally get the error
	// as well as the minimising point for the QEF
	glm::vec3 solve();

	float			AtA[6];
	glm::vec3		AtB;
	float			BtB;
	glm::vec3		masspoint;
	float			error;
};

#endif	// QEF_H_HAS_BEEN_INCLUDED

