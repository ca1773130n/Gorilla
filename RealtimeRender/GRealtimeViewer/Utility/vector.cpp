#include "stdafx.h"
#include "vector.h"
#include <cmath>
#include <algorithm>
using std::min;
using std::max;
using std::swap;
using std::sort;

using namespace std;

#define GLT_SIGN_OF_COL(i)		(((i) % 2) ? -1.0f : 1.0f) 
#define GLT_LINEAR_PT(M,d,i,j)	((M) + ((i) * (d)) + (j))

float GLTVecDot(GLfloat* V1, GLfloat* V2, int D)
{
	float ret = 0;
	for( int i=0; i < D; i++ )
	{
		ret += V1[i] * V2[i];
	}
	return ret;
}

void GLTMatInv(GLTMatrix M,GLTMatrix out)
{
	GLfloat det = GLTMatDet(M);
	if ( det != 0.0 )
	{
		out[0] = ((M[5]*M[10]*M[15]) +
					 (M[6]*M[11]*M[13]) +
					 (M[7]*M[9]*M[14]) -
					 (M[5]*M[11]*M[14]) -
					 (M[6]*M[9]*M[15]) -
					 (M[7]*M[10]*M[13]))/det;
		out[1] = ((M[1]*M[11]*M[14]) +
					 (M[2]*M[9]*M[15]) +
					 (M[3]*M[10]*M[13]) -
					 (M[1]*M[10]*M[15]) -
					 (M[2]*M[11]*M[13]) -
					 (M[3]*M[9]*M[14]))/det;
		out[2] = ((M[1]*M[6]*M[15]) +
					 (M[2]*M[7]*M[13]) +
					 (M[3]*M[5]*M[14]) -
					 (M[1]*M[7]*M[14]) -
					 (M[2]*M[5]*M[15]) -
					 (M[3]*M[6]*M[13]))/det;
		out[3] = ((M[1]*M[7]*M[10]) +
					 (M[2]*M[5]*M[11]) +
					 (M[3]*M[6]*M[9]) -
					 (M[1]*M[6]*M[11]) -
					 (M[2]*M[7]*M[9]) -
					 (M[3]*M[5]*M[10]))/det;
		out[4] = ((M[4]*M[11]*M[14]) +
					 (M[6]*M[8]*M[15]) +
					 (M[7]*M[10]*M[12]) -
					 (M[4]*M[10]*M[15]) -
					 (M[6]*M[11]*M[12]) -
					 (M[7]*M[8]*M[14]))/det;
		out[5] = ((M[0]*M[10]*M[15]) +
					 (M[2]*M[11]*M[12]) +
					 (M[3]*M[8]*M[14]) -
					 (M[0]*M[11]*M[14]) -
					 (M[2]*M[8]*M[15]) -
					 (M[3]*M[10]*M[12]))/det;
		out[6] = ((M[0]*M[7]*M[14]) +
					 (M[2]*M[4]*M[15]) +
					 (M[3]*M[6]*M[12]) -
					 (M[0]*M[6]*M[15]) -
					 (M[2]*M[7]*M[12]) -
					 (M[3]*M[4]*M[14]))/det;
		out[7] = ((M[0]*M[6]*M[11]) +
					 (M[2]*M[7]*M[8]) +
					 (M[3]*M[4]*M[10]) -
					 (M[0]*M[7]*M[10]) -
					 (M[2]*M[4]*M[11]) -
					 (M[3]*M[6]*M[8]))/det;
		out[8] = ((M[4]*M[9]*M[15]) +
					 (M[5]*M[11]*M[12]) +
					 (M[7]*M[8]*M[13]) -
					 (M[4]*M[11]*M[13]) -
					 (M[5]*M[8]*M[15]) -
					 (M[7]*M[9]*M[12]))/det;
		out[9] = ((M[0]*M[11]*M[13]) +
					 (M[1]*M[8]*M[15]) +
					 (M[3]*M[9]*M[12]) -
					 (M[0]*M[9]*M[15]) -
					 (M[1]*M[11]*M[12]) -
					 (M[3]*M[8]*M[13]))/det;
		out[10] = ((M[0]*M[5]*M[15]) +
					 (M[1]*M[7]*M[12]) +
					 (M[3]*M[4]*M[13]) -
					 (M[0]*M[7]*M[13]) -
					 (M[1]*M[4]*M[15]) -
					 (M[3]*M[5]*M[12]))/det;
		out[11] = ((M[0]*M[7]*M[9]) +
					 (M[1]*M[4]*M[11]) +
					 (M[3]*M[5]*M[8]) -
					 (M[0]*M[5]*M[11]) -
					 (M[1]*M[7]*M[8]) -
					 (M[3]*M[4]*M[9]))/det;
		out[12] = ((M[4]*M[10]*M[13]) +
					 (M[5]*M[8]*M[14]) +
					 (M[6]*M[9]*M[12]) -
					 (M[4]*M[9]*M[14]) -
					 (M[5]*M[10]*M[12]) -
					 (M[6]*M[8]*M[13]))/det;
		out[13] = ((M[0]*M[9]*M[14]) +
					 (M[1]*M[10]*M[12]) +
					 (M[2]*M[8]*M[13]) -
					 (M[0]*M[10]*M[13]) -
					 (M[1]*M[8]*M[14]) -
					 (M[2]*M[9]*M[12]))/det;
		out[14] = ((M[0]*M[6]*M[13]) +
					 (M[1]*M[4]*M[14]) +
					 (M[2]*M[5]*M[12]) -
					 (M[0]*M[5]*M[14]) -
					 (M[1]*M[6]*M[12]) -
					 (M[2]*M[4]*M[13]))/det;
		out[15] = ((M[0]*M[5]*M[10]) +
					 (M[1]*M[6]*M[8]) +
					 (M[2]*M[4]*M[9]) -
					 (M[0]*M[6]*M[9]) -
					 (M[1]*M[4]*M[10]) -
					 (M[2]*M[5]*M[8]))/det;
	}		
	else
	{
		int i,j;
		for(i = 0;i < 4;i++)
		{
			for(j = 0;j < 4;j++)
			{
				out[(4*i)+j] = 0.0;
			}
		}
	}
}

static GLfloat GLTMatDetAux(GLfloat *M, int D);

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecIsSame                                                         */
/*                                                                       */
/*     V1, V2         : Vectors to be compared.                          */
/*     D              : Dimension of vectors (2, 3, 4)                   */
/*                                                                       */
/*  description : Testing whether two vectors are the same               */
/*                                                                       */
/*  return : int : If the same vector return 1 else 0                    */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
int GLTVecIsSame(GLfloat *V1, GLfloat *V2, int D)
{
    int i;

    for (i = 0; i < D; i++) {
	if (V1[i] != V2[i]) return(GLT_FALSE);
    }

    return(GLT_TRUE);
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecCopy                                                           */
/*                                                                       */
/*     V1        : Source vector                                         */
/*     V2        : Target vector                                         */
/*     D     : Dimension of vectors (2, 3, 4)                            */
/*                                                                       */
/*  description : Copying a vector V1 to V2                              */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVecCopy(GLfloat *V1, GLfloat *V2, int D)
{
    int i;

    for (i = 0; i < D; i++) V2[i] = V1[i];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecAdd                                                            */
/*                                                                       */
/*     V1, V2         : Vectors to be added                              */
/*     V3             : Result vector                                    */
/*     D     : Dimension of vectors (2, 3, 4)                            */
/*                                                                       */
/*  description : Adding two vectors, V3 = V1 + V2                       */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVecAdd(GLfloat *V1, GLfloat *V2, GLfloat *V3, int D)
{
    int i;

    for (i = 0; i < D; i++) V3[i] = V1[i] + V2[i];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecSub                                                            */
/*                                                                       */
/*     V1, V2         : Vectors to subtract                              */
/*     V3             : Target vector                                    */
/*     D              : Dimension of vectors (2, 3, 4)                   */
/*                                                                       */
/*  description : Subtracting vector , V3 = V1 - V2                      */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVecSub(GLfloat *V1, GLfloat *V2, GLfloat *V3, int D)
{
    int i;

    for (i = 0; i < D; i++) V3[i] = V1[i] - V2[i];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecSMul                                                           */
/*                                                                       */
/*     V1       : Vector to be scalar multiplied.                        */
/*     A        : Scalar to be multipled to V1.                          */
/*     D     : Dimension of vectors (2, 3, 4)                            */
/*                                                                       */
/*  description : Scalar multiplication, V2 = A V1                       */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVecSMul(GLfloat *V1, GLfloat A, GLfloat *V2, int D)
{
    int i;

    for (i = 0; i < D; i++) V2[i] = V1[i] * A;
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecNeg                                                            */
/*                                                                       */
/*     V1        : Vector to be negated                                  */
/*     V2        : Target vector                                         */
/*     D     : Dimension of vectors (2, 3, 4)                            */
/*                                                                       */
/*  description : Negation of vector, V2 = -V1                           */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVecNeg(GLfloat *V1, GLfloat *V2, int D)
{
    int i;

    for (i = 0; i < D; i++) V2[i] = -V1[i];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecNorm                                                           */
/*                                                                       */
/*     V        : Vector to be computed its norm                         */
/*     D     : Dimension of vector (2, 3, 4)                             */
/*                                                                       */
/*  description : Computing norm of vector                               */
/*                                                                       */
/*  return : GLfloat : Norm of vector V                                    */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
GLfloat GLTVecNorm(GLfloat *V, int D)
{
    return(sqrt(GLTVecSProd(V, V, D)));
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecNormalize                                                      */
/*                                                                       */
/*     V        : Vector to be normalized                                */
/*     D     : Dimension of vector (2, 3, 4)                             */
/*                                                                       */
/*  description : Normalizing a vector. Note: This function directly     */
/*                convert the argument vector to normal form.            */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVecNormalize(GLfloat *V, int D)
{
    int i;
    GLfloat Norm = GLTVecNorm(V, D);

    for (i = 0; i < D; i++) V[i] /= Norm;
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecSProd                                                          */
/*                                                                       */
/*     V1, V2         : Vector to be scalar producted.                   */
/*     D     : Dimension of vectors (2, 3, 4)                            */
/*                                                                       */
/*  description : Computing scalar product of two vectors V1 and V2      */
/*                                                                       */
/*  return : GLfloat : Scalar product of V1 and V2                         */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
GLfloat GLTVecSProd(GLfloat *V1, GLfloat *V2, int D)
{
    int i;
    GLfloat Sum = 0;

    for (i = 0; i < D; i++) Sum += V1[i] * V2[i]; 

    return(Sum);
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecVProd2                                                         */
/*                                                                       */
/*     V1, V2 :            2D vectors                                    */
/*                                                                       */
/*  description : Computing vector product of two 2D vectors.            */
/*                V1 = (x1, y1),  V2 = (x2, y2),                         */
/*                scalar product = x1 * y2 - x2 * y1                     */
/*                                                                       */
/*  return : GLfloat : Scalar product of two 2D vectors                    */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
GLfloat GLTVecVProd2(GLTVector2 V1, GLTVector2 V2)
{
    return(V1[0] * V2[1] - V2[0] * V1[1]);
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecVProd3                                                         */
/*                                                                       */
/*     V1, V2            : Two 3D vectors to be vector producted         */
/*     V3                : Target vector                                 */
/*                                                                       */
/*  description : Vector product of 3D vectors : V3 = V1 x V2            */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVecVProd3(GLTVector3 V1, GLTVector3 V2, GLTVector3 V3)
{
    V3[0] = V1[1] * V2[2] - V1[2] * V2[1];
    V3[1] = V1[2] * V2[0] - V1[0] * V2[2];
    V3[2] = V1[0] * V2[1] - V1[1] * V2[0];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVec4To3                                                           */
/*                                                                       */
/*     V1            : A homogenous vector                               */
/*     V2            : Target vector                                     */
/*                                                                       */
/*  description : Coercing a homogenous vector V1 to 3D vector V2        */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : vector                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVec4To3(GLTVector4 V1, GLTVector3 V2)
{
    V2[0] = V1[0] / V1[3];
    V2[1] = V1[1] / V1[3];
    V2[2] = V1[2] / V1[3];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTMatUnit                                                           */
/*                                                                       */
/*     M           : Matrix to be unit matrix                            */
/*                                                                       */
/*  description : Making the matrix M to be unit                         */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTMatUnit(GLTMatrix M)
{
    int i, j;

    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    M[(4*i)+j] = (i == j) ? 1.0f : 0.0f;
	}
    }
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTMatCopy                                                           */
/*                                                                       */
/*     M1           : Source matrix                                      */
/*     M2           : Target matrix                                      */
/*                                                                       */
/*  description : Copying a matrix M1 to M2                              */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTMatCopy(GLTMatrix M1, GLTMatrix M2)
{
    int i, j;

    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++) M2[(4*i)+j] = M1[(4*i)+j];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTMatAdd                                                            */
/*                                                                       */
/*     M1, M2           : Two matrices to be added                       */
/*     M3               : Target matrix                                  */
/*                                                                       */
/*  description : Adding two matrices, M3 = M1 + M2                      */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTMatAdd(GLTMatrix M1, GLTMatrix M2, GLTMatrix M3)
{
    int i, j;

    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++) M3[(4*i)+j] = M1[(4*i)+j] + M2[(4*i)+j];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTMatSub                                                            */
/*                                                                       */
/*     M1, M2           : Two matrices to subtract                       */
/*     M3               : Target matrix                                  */
/*                                                                       */
/*  description : Subtracting a matrix from another, M3 = M1 - M2        */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTMatSub(GLTMatrix M1, GLTMatrix M2, GLTMatrix M3)
{
    int i, j;

    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++) M3[(4*i)+j] = M1[(4*i)+j] - M2[(4*i)+j];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTMatMul                                                            */
/*                                                                       */
/*     M1, M2           : Two matrices to multiplied                     */
/*     M3               : Target matrix                                  */
/*                                                                       */
/*  description : Matrix multiplication, M3 = M1 * M2                    */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTMatMul(GLTMatrix M1, GLTMatrix M2, GLTMatrix M3)
{
    int i, j, k;

    for (i = 0; i < 4; i++) {
	for (j = 0; j < 4; j++) {
	    M3[(4*i)+j] = 0.0;
	    for (k = 0; k < 4; k++) {
		M3[(4*i)+j] += M1[(4*i)+k] * M2[(4*k)+j];
	    }
	}
    }
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTMatTranspose                                                      */
/*                                                                       */
/*     M1           : Source matrix                                      */
/*     M2           : Target matrix                                      */
/*                                                                       */
/*  description : Transposing a matrix M1 to M2, $M2 = M1^{T}$           */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTMatTranspose(GLTMatrix M1, GLTMatrix M2)
{
    int i, j;

    for (i = 0; i < 4; i++)  
	for (j = 0; j < 4; j++) M2[(4*i)+j] = M1[(4*j)+i];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTMatDet                                                            */
/*                                                                       */
/*     M           : Source matrix                                       */
/*                                                                       */
/*  description : Computing determinant of a matrix M                    */
/*                                                                       */
/*  return : GLfloat : Determinant value |M| of M                          */
/*                                                                       */
/*  keywords : matrix                                                    */
/*                                                                       */
/*-----------------------------------------------------------------------*/
GLfloat GLTMatDet(GLTMatrix M)
{
	GLfloat temp[16];
	int i,j,pos = 0;
	for(i = 0;i < 4;i++)
	{
		for(j = 0;j < 4;j++)
		{
			temp[pos++] = M[(4*i)+j];
		}
	}
	
    return(GLTMatDetAux((GLfloat *)temp, 4));
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTMatVecMul                                                         */
/*                                                                       */
/*     M           : Matrix to multiplication                            */
/*     V1           : Vector to be multiplied to M                       */
/*     V2           : Target vector                                      */
/*                                                                       */
/*  description : Multiplication of matrix and vector, V2 = M V1         */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix, vector                                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
//void GLTMatVecMul(GLTMatrix M, GLTVector4 V1, GLTVector4 V2)
void GLTMatVecMul(GLTMatrix M, GLTVector4 V1, GLTVector4 V2)
{
    int i, j;

    for (i = 0; i < 4; i++) {
	V2[i] = 0.0;
	for (j = 0; j < 4; j++) V2[i] += M[(4*i)+j] * V1[j];
    }
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTVecMatMul                                                         */
/*                                                                       */
/*     V1            : Vector to multiplication                          */
/*     M             : Matrix to be multiplied to V1                     */
/*     V2            : Target vector                                     */
/*                                                                       */
/*  description : Multiplication of vector and matrix, V2 = V1 * M       */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix, vector                                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTVecMatMul(GLTVector4 V1, GLTMatrix M, GLTVector4 V2)
{
    int i, j;

    for (i = 0; i < 4; i++) {
	V2[i] = 0.0;
	for (j = 0; j < 4; j++) V2[i] += V1[j] * M[(4*j)+i]; 
    }
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTTrTrans3                                                          */
/*                                                                       */
/*     V1           : Vector to be translated                            */
/*     V2           : Target vector                                      */
/*     T            : Translation vector                                 */
/*                                                                       */
/*  description : Translation of vector V1 to V2 with tr vector T        */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix, vector, transformation                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTTrTrans3(GLTVector3 V1, GLTVector3 V2, GLTVector3 T)
{
    GLTVecAdd(V1, T, V2, 3); 
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTTrRot3                                                            */
/*                                                                       */
/*     V1                : Vector to be rotated                          */
/*     V2                : Target vector                                 */
/*     R                 : rotation angle (in degree)                    */
/*     Axis              : 0(x), 1(y), 2(z)                              */
/*                                                                       */
/*  description : Rotation of vector V1 to V2 with angle R w.r.t. Axis   */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix, vector, transformation                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTTrRot3(GLTVector3 V1, GLTVector3 V2, GLfloat R, int Axis)
{
    GLfloat 
	Angle = (float)GLT_TORAD(R), 
	Cos = cos(Angle),
	Sin = sin(Angle);

    switch (Axis) {
	case GLT_X: 
	    V2[0] = V1[0];
	    V2[1] = V1[1] * Cos - V1[2] * Sin; 
	    V2[2] = V1[1] * Sin + V1[2] * Cos; 
	    break;
	case GLT_Y:
	    V2[1] = V1[1];
	    V2[2] = V1[2] * Cos - V1[0] * Sin; 
	    V2[0] = V1[2] * Sin + V1[0] * Cos; 
	    break;
	case GLT_Z:
	    V2[2] = V1[2];
	    V2[0] = V1[0] * Cos - V1[1] * Sin; 
	    V2[1] = V1[0] * Sin + V1[1] * Cos; 
	    break;
    }
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTTrScale3                                                          */
/*                                                                       */
/*     V1                : Vector to be scaled                           */
/*     V2                : Target vector                                 */
/*     S                 : Scaling factor vector                         */
/*                                                                       */
/*  description : Scaling of vector V1 to V2 with scale factor S         */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix, vector, transformation                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTTrScale3(GLTVector3 V1, GLTVector3 V2, GLTVector3 S)
{
    int i;

    for (i = 0; i < 3; i++) 
	V2[i] = V1[i] * S[i];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTTrMakeMatTrans                                                    */
/*                                                                       */
/*     M               : Target matrix                                   */
/*     T               : Translation vector                              */
/*                                                                       */
/*  description : Making forward 3D translation matrix M                 */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix, vector, transformation                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTTrMakeMatTrans(GLTMatrix M, GLTVector3 T)
{
    int i;

    GLTMatUnit(M);
    for (i = 0; i < 3; i++) M[(4*i)+3] = T[i];
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTTrMakeMatRot                                                      */
/*                                                                       */
/*     M               : Target matrix                                   */
/*     R               : Rotation angle (in degree)                      */
/*     Axis            : 0(x), 1(y), 2(z)                                */
/*                                                                       */
/*  description : Making forward 3D rotation matrix M                    */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix, vector, transformation                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTTrMakeMatRot(GLTMatrix M, GLfloat R, int Axis)
{
    GLfloat 
	Angle = (float)GLT_TORAD(R),
	Cos = cos(Angle),
	Sin = sin(Angle);

    GLTMatUnit(M);

    switch (Axis) {
	case GLT_X:
	    M[5] = Cos;  M[6] = -Sin;
	    M[9] = Sin;  M[10] = Cos;
	    break;
	case GLT_Y:
	    M[0] = Cos;  M[2] = Sin;
	    M[8] = -Sin;  M[10] = Cos;
	    break;
	case GLT_Z:
	    M[0] = Cos;  M[1] = -Sin;
	    M[4] = Sin;  M[5] = Cos;
	    break;
    }
}

/*-----------------------------------------------------------------------*/
/*                                                                       */
/*  GLTTrMakeMatScale                                                    */
/*                                                                       */
/*     M            : Target matrix                                      */
/*     S            : Scale vector                                       */
/*                                                                       */
/*  description : Making forward 3D scale matrix M                       */
/*                                                                       */
/*  return : None                                                        */
/*                                                                       */
/*  keywords : matrix, vector, transformation                            */
/*                                                                       */
/*-----------------------------------------------------------------------*/
void GLTTrMakeMatScale(GLTMatrix M, GLTVector3 S)
{
    int i;
 
    GLTMatUnit(M);
    for (i = 0; i < 3; i++) M[(4*i)+i] = S[i];
}


/*-------*/
/*                                                                       */
/*  GLTMatDetAux                                                         */
/*                                                                       */
/*     M        : Source matrix (D x D)                                  */
/*     D         : Dimension of matrix M                                 */
/*                                                                       */
/*  description : Computing determinant of a D x D matrix M              */
/*                                                                       */
/*  return : GLfloat : Determinant value                                 */
/*                                                                       */
/*  keywords : matrix                                                    */
/*                                                                       */
/*-------*/
static GLfloat GLTMatDetAux(GLfloat *M, int D)
{
    int Col, i, j, k;
    GLfloat *TM, *PM1, *PM2, Det;

    if (D == 2) {
	return((*M) * (*(M + 3)) - (*(M + 1)) * (*(M + 2)));
    }

    TM = (GLfloat *)malloc(sizeof(GLfloat) * (D - 1) * (D - 1));

    Det = 0.0;

    for (Col = 0; Col < D; Col++) {
	for (i = 1; i < D; i++) {
	    for (j = 0, k = 0; j < D; j++) {
		if (j != Col) {
		    PM1 = GLT_LINEAR_PT(TM, D - 1 , i - 1, k); 
		    PM2 = GLT_LINEAR_PT(M, D, i, j); 
		    *PM1 = *PM2;
		    k++;
		}
	    }
	}
        PM1 = GLT_LINEAR_PT(M, D, 0, Col);
	Det += (*PM1) * GLT_SIGN_OF_COL(Col) * GLTMatDetAux(TM, D - 1);
    }

    free(TM);

    return(Det);
}













