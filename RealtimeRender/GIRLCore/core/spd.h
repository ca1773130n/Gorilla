/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   spd.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - spd.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_SPD_H
#define GIRL_SPD_H
// spd.h*

namespace girl
{

class GIRLCOREDLL  SPD {
public:
	SPD() { 
		nSamples = 0; 
		lambdaMin = lambdaMax = delta = invDelta = 0.;
		samples = NULL;
	}
    ~SPD() {}

    // samples the SPD by performing a linear interpolation on the data
    inline float sample(const float lambda) const {
      if (nSamples < 1 || lambda < lambdaMin || lambda > lambdaMax)
        return 0.f;

      // interpolate the two closest samples linearly
      const float x = (lambda - lambdaMin) * invDelta;
      const int b0 = Floor2Int(x);
      const int b1 = min(b0 + 1, nSamples - 1);
      const float dx = x - b0;
      return Lerp(dx, samples[b0], samples[b1]);
    }

    inline void sample(u_int n, const float lambda[], float *p) const {
      for (u_int i = 0; i < n; ++i) {
        if (nSamples < 1 || lambda[i] < lambdaMin || lambda[i] > lambdaMax)
          p[i] = 0.f;

        // interpolate the two closest samples linearly
        const float x = (lambda[i] - lambdaMin) * invDelta;
        const int b0 = Floor2Int(x);
        const int b1 = min(b0 + 1, nSamples - 1);
        const float dx = x - b0;
        p[i] = Lerp(dx, samples[b0], samples[b1]);
      }
    }

	float y();
	void AllocateSamples(int n);
	void FreeSamples();
	void Normalize();
	void Clamp();
	void Scale(float s);
	void Whitepoint(float temp);

protected:
	int nSamples;
    float lambdaMin, lambdaMax;
    float delta, invDelta;
    float *samples;

  };

}//namespace girl

#endif // GIRL_SPD_H
