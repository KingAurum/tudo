#pragma once
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

class Point3D {
public:
	double x, y, z;
};

class Quaternion {
public:
	double w;
	Point3D u;
	Quaternion() {
		w = 0;
		u.x = 0;
		u.y = 0;
		u.z = 0;
	}
	Quaternion(double real, double i, double j, double k) {
		w = real;
		u.x = i;
		u.y = j;
		u.z = k;
	}

	inline void Multiply(const Quaternion q) {
		Quaternion tmp;
		tmp.u.x = ((w * q.u.x) + (u.x * q.w) + (u.y * q.u.z) - (u.z * q.u.y));
		tmp.u.y = ((w * q.u.y) - (u.x * q.u.z) + (u.y * q.w) + (u.z * q.u.x));
		tmp.u.z = ((w * q.u.z) + (u.x * q.u.y) - (u.y * q.u.x) + (u.z * q.w));
		tmp.w = ((w * q.w) - (u.x * q.u.x) - (u.y * q.u.y) - (u.z * q.u.z));
		*this = tmp;
	}

	inline double Norm() {
		return sqrt(u.x * u.x + u.y * u.y + u.z * u.z + w * w);
	}

	inline void Conjugate() {
		u.x = -u.x;
		u.y = -u.y;
		u.z = -u.z;
	}

	inline void Inverse() {
		double norm = Norm();
		Conjugate();
		u.x /= norm;
		u.y /= norm;
		u.z /= norm;
		w /= norm;
	}


	void ExportToMatrix(float matrix[16]) {
		float wx, wy, wz, xx, yy, yz, xy, xz, zz;

		// adapted from Shoemake
		xx = u.x * u.x;
		xy = u.x * u.y;
		xz = u.x * u.z;
		yy = u.y * u.y;
		zz = u.z * u.z;
		yz = u.y * u.z;

		wx = w * u.x;
		wy = w * u.y;
		wz = w * u.z;

		matrix[0] = 1.0f - 2.0f * (yy + zz);
		matrix[4] = 2.0f * (xy - wz);
		matrix[8] = 2.0f * (xz + wy);
		matrix[12] = 0.0;

		matrix[1] = 2.0f * (xy + wz);
		matrix[5] = 1.0f - 2.0f * (xx + zz);
		matrix[9] = 2.0f * (yz - wx);
		matrix[13] = 0.0;

		matrix[2] = 2.0f * (xz - wy);
		matrix[6] = 2.0f * (yz + wx);
		matrix[10] = 1.0f - 2.0f * (xx + yy);
		matrix[14] = 0.0;

		matrix[3] = 0;
		matrix[7] = 0;
		matrix[11] = 0;
		matrix[15] = 1;
	}
};


//Quaternion q;
//q.Multiply(q.Multiply(new Quaternion(0, v[0], v[1], v[2])), q.Conjugate());

//Queremos saber uma posição de cada junta, então "classe":
class Joint {
public:
	Joint(float length) {
		this->length = length;
	}
	float length;
	Point3D pos;
};