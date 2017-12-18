/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   matrix4x4.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - matrix4x4.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_MATRIX4X4_H
#define GIRL_MATRIX4X4_H

namespace girl
{

class GIRLCOREDLL   Matrix4x4 {
public:
	// Matrix4x4 Public Methods
	Matrix4x4() {
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if (i == j) m[i][j] = 1.;
				else m[i][j] = 0.;
	}
	Matrix4x4(float mat[4][4]);
	Matrix4x4(float t00, float t01, float t02, float t03,
	          float t10, float t11, float t12, float t13,
	          float t20, float t21, float t22, float t23,
	          float t30, float t31, float t32, float t33);
	boost::shared_ptr<Matrix4x4> Transpose() const;
	void Print(ostream &os) const {
		os << "[ ";
		for (int i = 0; i < 4; ++i) {
			os << "[ ";
			for (int j = 0; j < 4; ++j)  {
				os << m[i][j];
				if (j != 3) os << ", ";
			}
			os << " ] ";
		}
		os << " ] ";
	}
	static boost::shared_ptr<Matrix4x4>
		Mul(const boost::shared_ptr<Matrix4x4> &m1,
	        const boost::shared_ptr<Matrix4x4> &m2) {
		float r[4][4];
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				r[i][j] = m1->m[i][0] * m2->m[0][j] +
				          m1->m[i][1] * m2->m[1][j] +
				          m1->m[i][2] * m2->m[2][j] +
				          m1->m[i][3] * m2->m[3][j];
		boost::shared_ptr<Matrix4x4> o (new Matrix4x4(r));
		return o;
	}
	boost::shared_ptr<Matrix4x4> Inverse() const;
	float m[4][4];
};

}//namespace girl

#endif //GIRL_MATRIX4X4_H
