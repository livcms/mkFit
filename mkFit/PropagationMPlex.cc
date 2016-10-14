#include "PropagationMPlex.h"

//#define DEBUG
#include "Debug.h"

//==============================================================================
// propagateLineToRMPlex
//==============================================================================

using namespace Matriplex;

void propagateLineToRMPlex(const MPlexLS &psErr,  const MPlexLV& psPar,
                           const MPlexHS &msErr,  const MPlexHV& msPar,
                                 MPlexLS &outErr,       MPlexLV& outPar,
                           const int      N_proc)
{
   // XXX Regenerate parts below with a script.

   const idx_t N  = NN;

#pragma simd
   for (int n = 0; n < NN; ++n)
   {

     const float cosA = (psPar[0 * N + n] * psPar[3 * N + n] + psPar[1 * N + n] * psPar[4 * N + n]) / ( std::sqrt( ( psPar[0 * N + n] * psPar[0 * N + n] + psPar[1 * N + n] * psPar[1 * N + n] ) * ( psPar[3 * N + n] * psPar[3 * N + n] + psPar[4 * N + n] * psPar[4 * N + n] ) ) );
     const float dr  = (hipo(msPar[0 * N + n], msPar[1 * N + n]) - hipo(psPar[0 * N + n], psPar[1 * N + n])) / cosA;

     dprint_np(n, "propagateLineToRMPlex dr=" << dr);

      const float pt  = hipo(psPar[3 * N + n], psPar[4 * N + n]);
      const float p   = dr / pt; // path
      const float psq = p * p;

      outPar[0 * N + n] = psPar[0 * N + n] + p * psPar[3 * N + n];
      outPar[1 * N + n] = psPar[1 * N + n] + p * psPar[4 * N + n];
      outPar[2 * N + n] = psPar[2 * N + n] + p * psPar[5 * N + n];
      outPar[3 * N + n] = psPar[3 * N + n];
      outPar[4 * N + n] = psPar[4 * N + n];
      outPar[5 * N + n] = psPar[5 * N + n];

      {
        const MPlexLS& A = psErr;
              MPlexLS& B = outErr;

        B.fArray[0 * N + n] = A.fArray[0 * N + n];
        B.fArray[1 * N + n] = A.fArray[1 * N + n];
        B.fArray[2 * N + n] = A.fArray[2 * N + n];
        B.fArray[3 * N + n] = A.fArray[3 * N + n];
        B.fArray[4 * N + n] = A.fArray[4 * N + n];
        B.fArray[5 * N + n] = A.fArray[5 * N + n];
        B.fArray[6 * N + n] = A.fArray[6 * N + n] + p * A.fArray[0 * N + n];
        B.fArray[7 * N + n] = A.fArray[7 * N + n] + p * A.fArray[1 * N + n];
        B.fArray[8 * N + n] = A.fArray[8 * N + n] + p * A.fArray[3 * N + n];
        B.fArray[9 * N + n] = A.fArray[9 * N + n] + p * (A.fArray[6 * N + n] + A.fArray[6 * N + n]) + psq * A.fArray[0 * N + n];
        B.fArray[10 * N + n] = A.fArray[10 * N + n] + p * A.fArray[1 * N + n];
        B.fArray[11 * N + n] = A.fArray[11 * N + n] + p * A.fArray[2 * N + n];
        B.fArray[12 * N + n] = A.fArray[12 * N + n] + p * A.fArray[4 * N + n];
        B.fArray[13 * N + n] = A.fArray[13 * N + n] + p * (A.fArray[7 * N + n] + A.fArray[10 * N + n]) + psq * A.fArray[1 * N + n];
        B.fArray[14 * N + n] = A.fArray[14 * N + n] + p * (A.fArray[11 * N + n] + A.fArray[11 * N + n]) + psq * A.fArray[2 * N + n];
        B.fArray[15 * N + n] = A.fArray[15 * N + n] + p * A.fArray[3 * N + n];
        B.fArray[16 * N + n] = A.fArray[16 * N + n] + p * A.fArray[4 * N + n];
        B.fArray[17 * N + n] = A.fArray[17 * N + n] + p * A.fArray[5 * N + n];
        B.fArray[18 * N + n] = A.fArray[18 * N + n] + p * (A.fArray[8 * N + n] + A.fArray[15 * N + n]) + psq * A.fArray[3 * N + n];
        B.fArray[19 * N + n] = A.fArray[19 * N + n] + p * (A.fArray[12 * N + n] + A.fArray[16 * N + n]) + psq * A.fArray[4 * N + n];
        B.fArray[20 * N + n] = A.fArray[20 * N + n] + p * (A.fArray[17 * N + n] + A.fArray[17 * N + n]) + psq * A.fArray[5 * N + n];
      }

      dprint_np(n, "propagateLineToRMPlex arrive at r=" << hipo(outPar[0 * N + n], outPar[1 * N + n]));
   }
}


//==============================================================================
// propagateHelixToRMPlex
//==============================================================================

namespace
{

void MultHelixProp(const MPlexLL& A, const MPlexLS& B, MPlexLL& C)
{
   // C = A * B

   typedef float T;
   const idx_t N  = NN;

   const T *a = A.fArray; ASSUME_ALIGNED(a, 64);
   const T *b = B.fArray; ASSUME_ALIGNED(b, 64);
         T *c = C.fArray; ASSUME_ALIGNED(c, 64);

#include "MultHelixProp.ah"
}

void MultHelixPropTransp(const MPlexLL& A, const MPlexLL& B, MPlexLS& C)
{
   // C = B * AT;

   typedef float T;
   const idx_t N  = NN;

   const T *a = A.fArray; ASSUME_ALIGNED(a, 64);
   const T *b = B.fArray; ASSUME_ALIGNED(b, 64);
         T *c = C.fArray; ASSUME_ALIGNED(c, 64);

#include "MultHelixPropTransp.ah"
}

void MultHelixPropEndcap(const MPlexLL& A, const MPlexLS& B, MPlexLL& C)
{
   // C = A * B

   typedef float T;
   const idx_t N  = NN;

   const T *a = A.fArray; ASSUME_ALIGNED(a, 64);
   const T *b = B.fArray; ASSUME_ALIGNED(b, 64);
         T *c = C.fArray; ASSUME_ALIGNED(c, 64);

#include "MultHelixPropEndcap.ah"
}

void MultHelixPropTranspEndcap(const MPlexLL& A, const MPlexLL& B, MPlexLS& C)
{
   // C = B * AT;

   typedef float T;
   const idx_t N  = NN;

   const T *a = A.fArray; ASSUME_ALIGNED(a, 64);
   const T *b = B.fArray; ASSUME_ALIGNED(b, 64);
         T *c = C.fArray; ASSUME_ALIGNED(c, 64);

#include "MultHelixPropTranspEndcap.ah"
}

inline void MultHelixPropTemp(const MPlexLL& A, const MPlexLL& B, MPlexLL& C, int n)
{
   // C = A * B

  typedef float T;
  const idx_t N  = NN;

  const T *a = A.fArray; ASSUME_ALIGNED(a, 64);
  const T *b = B.fArray; ASSUME_ALIGNED(b, 64);
  T *c = C.fArray; ASSUME_ALIGNED(c, 64);

  c[ 0*N+n] = a[ 0*N+n]*b[ 0*N+n] + a[ 1*N+n]*b[ 6*N+n] + a[ 2*N+n]*b[12*N+n] + a[ 4*N+n]*b[24*N+n];
  c[ 1*N+n] = a[ 0*N+n]*b[ 1*N+n] + a[ 1*N+n]*b[ 7*N+n] + a[ 2*N+n]*b[13*N+n] + a[ 4*N+n]*b[25*N+n];
  c[ 2*N+n] = a[ 2*N+n];
  c[ 3*N+n] = a[ 0*N+n]*b[ 3*N+n] + a[ 1*N+n]*b[ 9*N+n] + a[ 2*N+n]*b[15*N+n] + a[ 3*N+n] + a[ 4*N+n]*b[27*N+n];
  c[ 4*N+n] = a[ 0*N+n]*b[ 4*N+n] + a[ 1*N+n]*b[10*N+n] + a[ 4*N+n];
  c[ 5*N+n] = a[ 2*N+n]*b[17*N+n] + a[ 5*N+n];
  c[ 6*N+n] = a[ 6*N+n]*b[ 0*N+n] + a[ 7*N+n]*b[ 6*N+n] + a[ 8*N+n]*b[12*N+n] + a[10*N+n]*b[24*N+n];
  c[ 7*N+n] = a[ 6*N+n]*b[ 1*N+n] + a[ 7*N+n]*b[ 7*N+n] + a[ 8*N+n]*b[13*N+n] + a[10*N+n]*b[25*N+n];
  c[ 8*N+n] = a[ 8*N+n];
  c[ 9*N+n] = a[ 6*N+n]*b[ 3*N+n] + a[ 7*N+n]*b[ 9*N+n] + a[ 8*N+n]*b[15*N+n] + a[ 9*N+n] + a[10*N+n]*b[27*N+n];
  c[10*N+n] = a[ 6*N+n]*b[ 4*N+n] + a[ 7*N+n]*b[10*N+n] + a[10*N+n];
  c[11*N+n] = a[ 8*N+n]*b[17*N+n] + a[11*N+n];
  c[12*N+n] = a[12*N+n]*b[ 0*N+n] + a[13*N+n]*b[ 6*N+n] + a[14*N+n]*b[12*N+n] + a[16*N+n]*b[24*N+n];
  c[13*N+n] = a[12*N+n]*b[ 1*N+n] + a[13*N+n]*b[ 7*N+n] + a[14*N+n]*b[13*N+n] + a[16*N+n]*b[25*N+n];
  c[14*N+n] = a[14*N+n];
  c[15*N+n] = a[12*N+n]*b[ 3*N+n] + a[13*N+n]*b[ 9*N+n] + a[14*N+n]*b[15*N+n] + a[15*N+n] + a[16*N+n]*b[27*N+n];
  c[16*N+n] = a[12*N+n]*b[ 4*N+n] + a[13*N+n]*b[10*N+n] + a[16*N+n];
  c[17*N+n] = a[14*N+n]*b[17*N+n] + a[17*N+n];
  c[18*N+n] = a[18*N+n]*b[ 0*N+n] + a[19*N+n]*b[ 6*N+n] + a[20*N+n]*b[12*N+n] + a[22*N+n]*b[24*N+n];
  c[19*N+n] = a[18*N+n]*b[ 1*N+n] + a[19*N+n]*b[ 7*N+n] + a[20*N+n]*b[13*N+n] + a[22*N+n]*b[25*N+n];
  c[20*N+n] = a[20*N+n];
  c[21*N+n] = a[18*N+n]*b[ 3*N+n] + a[19*N+n]*b[ 9*N+n] + a[20*N+n]*b[15*N+n] + a[21*N+n] + a[22*N+n]*b[27*N+n];
  c[22*N+n] = a[18*N+n]*b[ 4*N+n] + a[19*N+n]*b[10*N+n] + a[22*N+n];
  c[23*N+n] = a[20*N+n]*b[17*N+n] + a[23*N+n];
  c[24*N+n] = a[24*N+n]*b[ 0*N+n] + a[25*N+n]*b[ 6*N+n] + a[26*N+n]*b[12*N+n] + a[28*N+n]*b[24*N+n];
  c[25*N+n] = a[24*N+n]*b[ 1*N+n] + a[25*N+n]*b[ 7*N+n] + a[26*N+n]*b[13*N+n] + a[28*N+n]*b[25*N+n];
  c[26*N+n] = a[26*N+n];
  c[27*N+n] = a[24*N+n]*b[ 3*N+n] + a[25*N+n]*b[ 9*N+n] + a[26*N+n]*b[15*N+n] + a[27*N+n] + a[28*N+n]*b[27*N+n];
  c[28*N+n] = a[24*N+n]*b[ 4*N+n] + a[25*N+n]*b[10*N+n] + a[28*N+n];
  c[29*N+n] = a[26*N+n]*b[17*N+n] + a[29*N+n];
  c[30*N+n] = a[30*N+n]*b[ 0*N+n] + a[31*N+n]*b[ 6*N+n] + a[32*N+n]*b[12*N+n] + a[34*N+n]*b[24*N+n];
  c[31*N+n] = a[30*N+n]*b[ 1*N+n] + a[31*N+n]*b[ 7*N+n] + a[32*N+n]*b[13*N+n] + a[34*N+n]*b[25*N+n];
  c[32*N+n] = a[32*N+n];
  c[33*N+n] = a[30*N+n]*b[ 3*N+n] + a[31*N+n]*b[ 9*N+n] + a[32*N+n]*b[15*N+n] + a[33*N+n] + a[34*N+n]*b[27*N+n];
  c[34*N+n] = a[30*N+n]*b[ 4*N+n] + a[31*N+n]*b[10*N+n] + a[34*N+n];
  c[35*N+n] = a[32*N+n]*b[17*N+n] + a[35*N+n];
}

// this version does not assume to know which elements are 0 or 1, so it does the full multiplication
void MultHelixPropFull(const MPlexLL& A, const MPlexLS& B, MPlexLL& C)
{
#pragma simd
  for (int n = 0; n < NN; ++n)
    {
      for (int i = 0; i < 6; ++i) {
	for (int j = 0; j < 6; ++j) {
	  C(n,i,j) = 0.;
	  for (int k = 0; k < 6; ++k) C(n,i,j) += A.ConstAt(n,i,k)*B.ConstAt(n,k,j);
	}
      }
    }
}

// this version does not assume to know which elements are 0 or 1, so it does the full multiplication
void MultHelixPropFull(const MPlexLL& A, const MPlexLL& B, MPlexLL& C)
{
#pragma simd
  for (int n = 0; n < NN; ++n)
    {
      for (int i = 0; i < 6; ++i) {
	for (int j = 0; j < 6; ++j) {
	  C(n,i,j) = 0.;
	  for (int k = 0; k < 6; ++k) C(n,i,j) += A.ConstAt(n,i,k)*B.ConstAt(n,k,j);
	}
      }
    }
}

// this version does not assume to know which elements are 0 or 1, so it does the full mupltiplication
void MultHelixPropTranspFull(const MPlexLL& A, const MPlexLL& B, MPlexLS& C)
{
#pragma simd
  for (int n = 0; n < NN; ++n)
    {
      for (int i = 0; i < 6; ++i) {
	for (int j = 0; j < 6; ++j) {
	  C(n,i,j) = 0.;
	  for (int k = 0; k < 6; ++k) C(n,i,j) += B.ConstAt(n,i,k)*A.ConstAt(n,j,k);
	}
      }
    }
}

// this version does not assume to know which elements are 0 or 1, so it does the full mupltiplication
void MultHelixPropTranspFull(const MPlexLL& A, const MPlexLL& B, MPlexLL& C)
{
#pragma simd
  for (int n = 0; n < NN; ++n)
    {
      for (int i = 0; i < 6; ++i) {
	for (int j = 0; j < 6; ++j) {
	  C(n,i,j) = 0.;
	  for (int k = 0; k < 6; ++k) C(n,i,j) += B.ConstAt(n,i,k)*A.ConstAt(n,j,k);
	}
      }
    }
}

} // end unnamed namespace

void helixAtRFromIterativeCCSFullJac(const MPlexLV& inPar, const MPlexQI& inChg, MPlexLV& outPar,
                                     const MPlexQF &msRad,       MPlexLL& errorProp,
                                     const int      N_proc)
{
  errorProp.SetVal(0.f);
  MPlexLL errorPropTmp(0.f);//initialize to zero
  MPlexLL errorPropSwap(0.f);//initialize to zero

#pragma simd
  for (int n = 0; n < NN; ++n)
    {

      //initialize erroProp to identity matrix
      errorProp(n,0,0) = 1.f;
      errorProp(n,1,1) = 1.f;
      errorProp(n,2,2) = 1.f;
      errorProp(n,3,3) = 1.f;
      errorProp(n,4,4) = 1.f;
      errorProp(n,5,5) = 1.f;

      const float k = inChg.ConstAt(n, 0, 0) * 100.f / (-Config::sol*Config::Bfield);
      const float r = msRad.ConstAt(n, 0, 0);
      float r0 = hipo(inPar.ConstAt(n, 0, 0), inPar.ConstAt(n, 1, 0));

      if (std::abs(r-r0) < 0.0001f) {
	dprint_np(n, "distance less than 1mum, skip");
	continue;
      }

      const float xin   = inPar.ConstAt(n, 0, 0);
      const float yin   = inPar.ConstAt(n, 1, 0);
      const float zin   = inPar.ConstAt(n, 2, 0);
      const float ipt   = inPar.ConstAt(n, 3, 0);
      const float phiin = inPar.ConstAt(n, 4, 0);
      const float theta = inPar.ConstAt(n, 5, 0);

      //set those that are 1. before iterations
      errorPropTmp(n,2,2) = 1.f;
      errorPropTmp(n,3,3) = 1.f;
      errorPropTmp(n,4,4) = 1.f;
      errorPropTmp(n,5,5) = 1.f;

      float cosa = 0., sina = 0.;
      //no trig approx here, phi and theta can be large
            float cosP = std::cos(phiin), sinP = std::sin(phiin);
      const float cosT = std::cos(theta), sinT = std::sin(theta);
      float pxin = cosP/ipt;
      float pyin = sinP/ipt;

      for (int i = 0; i < Config::Niter; ++i)
      {
	dprint_np(n, std::endl << "attempt propagation from r=" << r0 << " to r=" << r << std::endl
	       << "x=" << outPar.At(n, 0, 0) << " y=" << outPar.At(n, 1, 0)  << " z=" << outPar.At(n, 2, 0)
	       << " px=" << std::cos(phiin)/ipt << " py=" << std::sin(phiin)/ipt << " pz=" << 1.f/(ipt*tan(theta)) << " q=" << inChg.ConstAt(n, 0, 0) << std::endl);

	r0 = hipo(outPar.ConstAt(n, 0, 0), outPar.ConstAt(n, 1, 0));
	const float ialpha = (r-r0)*ipt/k;
	//alpha+=ialpha;

	if (Config::useTrigApprox) {
	  sincos4(ialpha, sina, cosa);
	} else {
	  cosa=std::cos(ialpha);
	  sina=std::sin(ialpha);
	}

	//derivatives of alpha
	const float dadx   = -outPar.At(n, 0, 0)*ipt/(k*r0);
	const float dady   = -outPar.At(n, 1, 0)*ipt/(k*r0);
	const float dadipt = (r-r0)/k;

	outPar.At(n, 0, 0) = outPar.ConstAt(n, 0, 0) + k*(pxin*sina-pyin*(1.f-cosa));
	outPar.At(n, 1, 0) = outPar.ConstAt(n, 1, 0) + k*(pyin*sina+pxin*(1.f-cosa));
	const float pxinold = pxin;//copy before overwriting
	pxin = pxin*cosa-pyin*sina;
	pyin = pyin*cosa+pxinold*sina;

	//need phi at origin, so this goes before redefining phi
	//no trig approx here, phi can be large
	cosP=std::cos(outPar.At(n, 4, 0));
	sinP=std::sin(outPar.At(n, 4, 0));

	outPar.At(n, 2, 0) = outPar.ConstAt(n, 2, 0) + k*ialpha*cosT/(ipt*sinT);
	outPar.At(n, 3, 0) = ipt;
	outPar.At(n, 4, 0) = outPar.ConstAt(n, 4, 0)+ialpha;
	outPar.At(n, 5, 0) = theta;

	errorPropTmp(n,0,0) = 1.f+k*(cosP*dadx*cosa-sinP*dadx*sina)/ipt;
	errorPropTmp(n,0,1) =     k*(cosP*dady*cosa-sinP*dady*sina)/ipt;
	errorPropTmp(n,0,3) = k*(cosP*(ipt*dadipt*cosa-sina)+sinP*((1.f-cosa)-ipt*dadipt*sina))/(ipt*ipt);
	errorPropTmp(n,0,4) =-k*(sinP*sina+cosP*(1.f-cosa))/ipt;

	errorPropTmp(n,1,0) =     k*(sinP*dadx*cosa+cosP*dadx*sina)/ipt;
	errorPropTmp(n,1,1) = 1.f+k*(sinP*dady*cosa+cosP*dady*sina)/ipt;
	errorPropTmp(n,1,3) = k*(sinP*(ipt*dadipt*cosa-sina)+cosP*(ipt*dadipt*sina-(1.f-cosa)))/(ipt*ipt);
	errorPropTmp(n,1,4) = k*(cosP*sina-sinP*(1.f-cosa))/ipt;

	errorPropTmp(n,2,0) = k*cosT*dadx/(ipt*sinT);
	errorPropTmp(n,2,1) = k*cosT*dady/(ipt*sinT);
	errorPropTmp(n,2,3) = k*cosT*(ipt*dadipt-ialpha)/(ipt*ipt*sinT);
	errorPropTmp(n,2,5) =-k*ialpha/(ipt*sinT*sinT);

	errorPropTmp(n,4,0) = dadx;
	errorPropTmp(n,4,1) = dady;
	errorPropTmp(n,4,3) = dadipt;

	MultHelixPropTemp(errorProp,errorPropTmp,errorPropSwap,n);
	errorProp = errorPropSwap;
      }

      dprint_np(n, "propagation end, dump parameters" << std::endl
	     << "pos = " << outPar.At(n, 0, 0) << " " << outPar.At(n, 1, 0) << " " << outPar.At(n, 2, 0) << std::endl
	     << "mom = " << std::cos(outPar.At(n, 4, 0))/outPar.At(n, 3, 0) << " " << std::sin(outPar.At(n, 4, 0))/outPar.At(n, 3, 0) << " " << 1./(outPar.At(n, 3, 0)*tan(outPar.At(n, 5, 0)))
	     << " r=" << std::sqrt( outPar.At(n, 0, 0)*outPar.At(n, 0, 0) + outPar.At(n, 1, 0)*outPar.At(n, 1, 0) ) << " pT=" << 1./std::abs(outPar.At(n, 3, 0)) << std::endl);
      
#ifdef DEBUG
      if (n < N_proc)
      {
	dmutex_guard;
	std::cout << n << " jacobian" << std::endl;
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,0,0),errorProp(n,0,1),errorProp(n,0,2),errorProp(n,0,3),errorProp(n,0,4),errorProp(n,0,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,1,0),errorProp(n,1,1),errorProp(n,1,2),errorProp(n,1,3),errorProp(n,1,4),errorProp(n,1,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,2,0),errorProp(n,2,1),errorProp(n,2,2),errorProp(n,2,3),errorProp(n,2,4),errorProp(n,2,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,3,0),errorProp(n,3,1),errorProp(n,3,2),errorProp(n,3,3),errorProp(n,3,4),errorProp(n,3,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,4,0),errorProp(n,4,1),errorProp(n,4,2),errorProp(n,4,3),errorProp(n,4,4),errorProp(n,4,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,5,0),errorProp(n,5,1),errorProp(n,5,2),errorProp(n,5,3),errorProp(n,5,4),errorProp(n,5,5));
      }
#endif
    }
}

void helixAtRFromIterativeCCS(const MPlexLV& inPar, const MPlexQI& inChg, MPlexLV& outPar,
                              const MPlexQF &msRad, MPlexLL& errorProp,
                              const int      N_proc)
{
  errorProp.SetVal(0.f);

#pragma simd
  for (int n = 0; n < NN; ++n)
    {
      //initialize erroProp to identity matrix
      errorProp(n,0,0) = 1.f;
      errorProp(n,1,1) = 1.f;
      errorProp(n,2,2) = 1.f;
      errorProp(n,3,3) = 1.f;
      errorProp(n,4,4) = 1.f;
      errorProp(n,5,5) = 1.f;

      const float k = inChg.ConstAt(n, 0, 0) * 100.f / (-Config::sol*Config::Bfield);
      const float r = msRad.ConstAt(n, 0, 0);
      float r0 = hipo(inPar.ConstAt(n, 0, 0), inPar.ConstAt(n, 1, 0));

      // if (std::abs(r-r0)<0.0001f) {
      // 	dprint("distance less than 1mum, skip");
      // 	continue;
      // }

      const float xin   = inPar.ConstAt(n, 0, 0);
      const float yin   = inPar.ConstAt(n, 1, 0);
      const float zin   = inPar.ConstAt(n, 2, 0);
      const float ipt   = inPar.ConstAt(n, 3, 0);
      const float phiin = inPar.ConstAt(n, 4, 0);
      const float theta = inPar.ConstAt(n, 5, 0);

      dprint_np(n, std::endl << "input parameters"
            << " inPar.ConstAt(n, 0, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 0, 0)
            << " inPar.ConstAt(n, 1, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 1, 0)
            << " inPar.ConstAt(n, 2, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 2, 0)
            << " inPar.ConstAt(n, 3, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 3, 0)
            << " inPar.ConstAt(n, 4, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 4, 0)
            << " inPar.ConstAt(n, 5, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 5, 0)
            );

      const float kinv  = 1.f/k;
      const float pt = 1.f/ipt;

      float D = 0., cosa = 0., sina = 0., id = 0.;
      //no trig approx here, phi can be large
      float cosPorT = std::cos(phiin), sinPorT = std::sin(phiin);
      float pxin = cosPorT*pt;
      float pyin = sinPorT*pt;

      dprint_np(n, std::endl << "k=" << std::setprecision(9) << k << " pxin=" << std::setprecision(9) << pxin << " pyin="
             << std::setprecision(9) << pyin << " cosPorT=" << std::setprecision(9) << cosPorT
             << " sinPorT=" << std::setprecision(9) << sinPorT << " pt=" << std::setprecision(9) << pt);

      //derivatives initialized to value for first iteration, i.e. distance = r-r0in
      float dDdx = r0 > 0.f ? -xin/r0 : 0.f;
      float dDdy = r0 > 0.f ? -yin/r0 : 0.f;
      float dDdipt = 0.;
      float dDdphi = 0.;

      for (int i = 0; i < Config::Niter; ++i)
      {
	dprint_np(n, std::endl << "attempt propagation from r=" << r0 << " to r=" << r << std::endl
	       << "x=" << xin << " y=" << yin  << " z=" << inPar.ConstAt(n, 2, 0) << " px=" << pxin << " py=" << pyin << " pz=" << pt*std::tan(theta) << " q=" << inChg.ConstAt(n, 0, 0));

	//compute distance and path for the current iteration
	r0 = hipo(outPar.At(n, 0, 0), outPar.At(n, 1, 0));
	id = (r-r0);
	D+=id;
	if (Config::useTrigApprox) {
	  sincos4(id*ipt*kinv, sina, cosa);
	} else {
          cosa=std::cos(id*ipt*kinv);
          sina=std::sin(id*ipt*kinv);
	}

        dprint_np(n, std::endl << "r=" << std::setprecision(9) << r << " r0=" << std::setprecision(9) << r0
               << " id=" << std::setprecision(9) << id << " cosa=" << cosa << " sina=" << sina);

	//update derivatives on total distance
	if (i+1 != Config::Niter) {

          const float x = outPar.At(n, 0, 0);
          const float y = outPar.At(n, 1, 0);
          const float oor0 = (r0>0.f && std::abs(r-r0)<0.0001f) ? 1.f/r0 : 0.f;

          const float dadipt = id*kinv;

          const float dadx = -x*ipt*kinv*oor0;
          const float dady = -y*ipt*kinv*oor0;

	  const float pxca = pxin*cosa;
	  const float pxsa = pxin*sina;
	  const float pyca = pyin*cosa;
	  const float pysa = pyin*sina;

	  float tmp = k*dadx;
          dDdx   -= ( x*(1.f + tmp*(pxca - pysa)) + y*tmp*(pyca + pxsa) )*oor0;
	  tmp = k*dady;
          dDdy   -= ( x*tmp*(pxca - pysa) + y*(1.f + tmp*(pyca + pxsa)) )*oor0;
          //now r0 depends on ipt and phi as well
	  tmp = dadipt*ipt;
          dDdipt -= k*( x*(pxca*tmp - pysa*tmp - pyca - pxsa + pyin) +
                        y*(pyca*tmp + pxsa*tmp - pysa + pxca - pxin))*pt*oor0;
          dDdphi += k*( x*(pysa - pxin + pxca) - y*(pxsa - pyin + pyca))*oor0;
        }

	//update parameters
	outPar.At(n, 0, 0) = outPar.At(n, 0, 0) + k*(pxin*sina - pyin*(1.f-cosa));
	outPar.At(n, 1, 0) = outPar.At(n, 1, 0) + k*(pyin*sina + pxin*(1.f-cosa));
	const float pxinold = pxin;//copy before overwriting
	pxin = pxin*cosa - pyin*sina;
	pyin = pyin*cosa + pxinold*sina;

        dprint_np(n, std::endl << "outPar.At(n, 0, 0)=" << outPar.At(n, 0, 0) << " outPar.At(n, 1, 0)=" << outPar.At(n, 1, 0)
               << " pxin=" << pxin << " pyin=" << pyin);
      }

      const float alpha  = D*ipt*kinv;
      const float dadx   = dDdx*ipt*kinv;
      const float dady   = dDdy*ipt*kinv;
      const float dadipt = (ipt*dDdipt + D)*kinv;
      const float dadphi = dDdphi*ipt*kinv;

      if (Config::useTrigApprox) {
	sincos4(alpha, sina, cosa);
      } else {
	cosa=std::cos(alpha);
	sina=std::sin(alpha);
      }

      errorProp(n,0,0) = 1.f+k*dadx*(cosPorT*cosa-sinPorT*sina)*pt;
      errorProp(n,0,1) =     k*dady*(cosPorT*cosa-sinPorT*sina)*pt;
      errorProp(n,0,2) = 0.f;
      errorProp(n,0,3) = k*(cosPorT*(ipt*dadipt*cosa-sina)+sinPorT*((1.f-cosa)-ipt*dadipt*sina))*pt*pt;
      errorProp(n,0,4) = k*(cosPorT*dadphi*cosa - sinPorT*dadphi*sina - sinPorT*sina + cosPorT*cosa - cosPorT)*pt;
      errorProp(n,0,5) = 0.f;

      errorProp(n,1,0) =     k*dadx*(sinPorT*cosa+cosPorT*sina)*pt;
      errorProp(n,1,1) = 1.f+k*dady*(sinPorT*cosa+cosPorT*sina)*pt;
      errorProp(n,1,2) = 0.f;
      errorProp(n,1,3) = k*(sinPorT*(ipt*dadipt*cosa-sina)+cosPorT*(ipt*dadipt*sina-(1.f-cosa)))*pt*pt;
      errorProp(n,1,4) = k*(sinPorT*dadphi*cosa + cosPorT*dadphi*sina + sinPorT*cosa + cosPorT*sina - sinPorT)*pt;
      errorProp(n,1,5) = 0.f;

      //no trig approx here, theta can be large
      cosPorT=std::cos(theta);
      sinPorT=std::sin(theta);
      //redefine sinPorT as 1./sinPorT to reduce the number of temporaries
      sinPorT = 1.f/sinPorT;

      outPar.At(n, 2, 0) = inPar.ConstAt(n, 2, 0) + k*alpha*cosPorT*pt*sinPorT;

      errorProp(n,2,0) = k*cosPorT*dadx*pt*sinPorT;
      errorProp(n,2,1) = k*cosPorT*dady*pt*sinPorT;
      errorProp(n,2,2) = 1.f;
      errorProp(n,2,3) = k*cosPorT*(ipt*dadipt-alpha)*pt*pt*sinPorT;
      errorProp(n,2,4) = k*dadphi*cosPorT*pt*sinPorT;
      errorProp(n,2,5) =-k*alpha*pt*sinPorT*sinPorT;

      outPar.At(n, 3, 0) = ipt;

      errorProp(n,3,0) = 0.f;
      errorProp(n,3,1) = 0.f;
      errorProp(n,3,2) = 0.f;
      errorProp(n,3,3) = 1.f;
      errorProp(n,3,4) = 0.f;
      errorProp(n,3,5) = 0.f;

      outPar.At(n, 4, 0) = inPar.ConstAt(n, 4, 0)+alpha;

      errorProp(n,4,0) = dadx;
      errorProp(n,4,1) = dady;
      errorProp(n,4,2) = 0.f;
      errorProp(n,4,3) = dadipt;
      errorProp(n,4,4) = 1.f+dadphi;
      errorProp(n,4,5) = 0.f;

      outPar.At(n, 5, 0) = theta;

      errorProp(n,5,0) = 0.f;
      errorProp(n,5,1) = 0.f;
      errorProp(n,5,2) = 0.f;
      errorProp(n,5,3) = 0.f;
      errorProp(n,5,4) = 0.f;
      errorProp(n,5,5) = 1.f;

      dprint_np(n, "propagation end, dump parameters" << std::endl
	     << "pos = " << outPar.At(n, 0, 0) << " " << outPar.At(n, 1, 0) << " " << outPar.At(n, 2, 0) << std::endl
	     << "mom = " << std::cos(outPar.At(n, 4, 0))/outPar.At(n, 3, 0) << " " << std::sin(outPar.At(n, 4, 0))/outPar.At(n, 3, 0) << " " << 1./(outPar.At(n, 3, 0)*tan(outPar.At(n, 5, 0)))
	     << " r=" << std::sqrt( outPar.At(n, 0, 0)*outPar.At(n, 0, 0) + outPar.At(n, 1, 0)*outPar.At(n, 1, 0) ) << " pT=" << 1./std::abs(outPar.At(n, 3, 0)) << std::endl);
      
#ifdef DEBUG
      if (n < N_proc) {
	dmutex_guard;
	std::cout << n << ": jacobian" << std::endl;
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,0,0),errorProp(n,0,1),errorProp(n,0,2),errorProp(n,0,3),errorProp(n,0,4),errorProp(n,0,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,1,0),errorProp(n,1,1),errorProp(n,1,2),errorProp(n,1,3),errorProp(n,1,4),errorProp(n,1,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,2,0),errorProp(n,2,1),errorProp(n,2,2),errorProp(n,2,3),errorProp(n,2,4),errorProp(n,2,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,3,0),errorProp(n,3,1),errorProp(n,3,2),errorProp(n,3,3),errorProp(n,3,4),errorProp(n,3,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,4,0),errorProp(n,4,1),errorProp(n,4,2),errorProp(n,4,3),errorProp(n,4,4),errorProp(n,4,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,5,0),errorProp(n,5,1),errorProp(n,5,2),errorProp(n,5,3),errorProp(n,5,4),errorProp(n,5,5));
      }
#endif
    }
}

#if 0
// FIXME: Do not destroy improvements from the devel branch.
//        Dan's shared .icc proof of concept should be bring back once everything is cleanly merged.
//#pragma omp declare simd simdlen(NN) notinbranch linear(n)
#include "PropagationMPlex.icc"

void helixAtRFromIterative(const MPlexLV& inPar, const MPlexQI& inChg, MPlexLV& outPar, const MPlexQF &msRad, MPlexLL& errorProp) {
  errorProp.SetVal(0);

  //#pragma ivdep
  helixAtRFromIterative_impl(inPar, inChg, outPar, msRad, errorProp, 0, NN);
#endif
void helixAtRFromIterative(const MPlexLV& inPar, const MPlexQI& inChg, MPlexLV& outPar,
                           const MPlexQF &msRad, MPlexLL& errorProp,
                           const int      N_proc)
{
  errorProp.SetVal(0.f);

#pragma simd
  for (int n = 0; n < NN; ++n)
    {

      //initialize erroProp to identity matrix
      errorProp(n,0,0) = 1.f;
      errorProp(n,1,1) = 1.f;
      errorProp(n,2,2) = 1.f;
      errorProp(n,3,3) = 1.f;
      errorProp(n,4,4) = 1.f;
      errorProp(n,5,5) = 1.f;

      const float xin  = inPar.ConstAt(n, 0, 0);
      const float yin  = inPar.ConstAt(n, 1, 0);
      const float pxin = inPar.ConstAt(n, 3, 0);
      const float pyin = inPar.ConstAt(n, 4, 0);
      const float pzin = inPar.ConstAt(n, 5, 0);
      const float r    = msRad.ConstAt(n, 0, 0);
      float r0 = hipo(xin, yin);
      
      dprint_np(n, std::endl << "attempt propagation from r=" << r0 << " to r=" << r << std::endl
        << "x=" << xin << " y=" << yin  << " z=" << inPar.ConstAt(n, 2, 0) << " px=" << pxin << " py=" << pyin << " pz=" << pzin << " q=" << inChg.ConstAt(n, 0, 0));

      if (std::abs(r-r0)<0.0001f) {
	dprint_np(n, "distance less than 1mum, skip");
	continue;
      }
      
      const float pt2    = pxin*pxin+pyin*pyin;
      const float pt     = std::sqrt(pt2);
      const float ptinv  = 1.f/pt;
      const float pt2inv = ptinv*ptinv;
      //p=0.3Br => r=p/(0.3*B)
      const float k = inChg.ConstAt(n, 0, 0) * 100.f / (-Config::sol*Config::Bfield);
      const float invcurvature = 1.f/(pt*k);//in 1./cm
      const float ctgTheta=pzin*ptinv;
      
      dprint_np(n, "curvature=" << 1.f/invcurvature);
      
      //variables to be updated at each iterations
      float totalDistance = 0;
      //derivatives initialized to value for first iteration, i.e. distance = r-r0in
      float dTDdx = r0>0.f ? -xin/r0 : 0.f;
      float dTDdy = r0>0.f ? -yin/r0 : 0.f;
      float dTDdpx = 0.;
      float dTDdpy = 0.;
      for (int i = 0; i < Config::Niter; ++i)
	{
	  dprint_np(n, "propagation iteration #" << i);
	  
	  const float x  = outPar.At(n, 0, 0);
	  const float y  = outPar.At(n, 1, 0);
	  const float px = outPar.At(n, 3, 0);
	  const float py = outPar.At(n, 4, 0);
	  r0 = hipo(outPar.At(n, 0, 0), outPar.At(n, 1, 0));
	  
	  dprint_np(n, "r0=" << r0 << " pt=" << pt);
	  
	  //distance=r-r0;//remove temporary
	  totalDistance+=(r-r0);
	  
	  dprint_np(n, "distance=" << (r-r0) << " angPath=" << (r-r0)*invcurvature);
	  
	  //float angPath = (r-r0)*invcurvature;
          float cosAP, sinAP;
	  if (Config::useTrigApprox) {
	    sincos4((r-r0)*invcurvature, sinAP, cosAP);
	  } else {
	    cosAP=std::cos((r-r0)*invcurvature);
	    sinAP=std::sin((r-r0)*invcurvature);
	  }

	  //helix propagation formulas
	  //http://www.phys.ufl.edu/~avery/fitting/fitting4.pdf
	  outPar.At(n, 0, 0) = outPar.At(n, 0, 0) + k*(px*sinAP-py*(1-cosAP));
	  outPar.At(n, 1, 0) = outPar.At(n, 1, 0) + k*(py*sinAP+px*(1-cosAP));
	  outPar.At(n, 2, 0) = outPar.At(n, 2, 0) + (r-r0)*ctgTheta;
	  outPar.At(n, 3, 0) = px*cosAP-py*sinAP;
	  outPar.At(n, 4, 0) = py*cosAP+px*sinAP;
	  //outPar.At(n, 5, 0) = pz; //take this out as it is redundant

	  if (i+1 != Config::Niter && r0 > 0 && std::abs((r-r0)*invcurvature)>0.000000001f)
	  {
	     //update derivatives on total distance for next step, where totalDistance+=r-r0
	     //now r0 depends on px and py

	     dprint_np(n, "r0=" << 1.f/r0 << " r0inv=" << r0 << " pt=" << pt);

	     //update derivative on D
	     const float dAPdpx = -(r-r0)*invcurvature*px*pt2inv;//r0 is now 1./r0 (this could go above the redefinition of r0!)
	     const float dAPdpy = -(r-r0)*invcurvature*py*pt2inv;
	     r0 = 1.f/r0;//WARNING, now r0 is r0inv (one less temporary)
	     const float dAPdx = -x*r0*invcurvature;
	     const float dAPdy = -y*r0*invcurvature;
	     //reduce temporary variables
	     //dxdx = 1 + k*dAPdx*(px*cosAP - py*sinAP);
	     //dydx = k*dAPdx*(py*cosAP + px*sinAP);
	     //dTDdx -= r0*(x*dxdx + y*dydx);
	     dTDdx -= r0*(x*(1.f + k*dAPdx*(px*cosAP - py*sinAP)) + y*(k*dAPdx*(py*cosAP + px*sinAP)));
	     //reuse same temporary variables
	     //dxdy = k*dAPdy*(px*cosAP - py*sinAP);
	     //dydy = 1 + k*dAPdy*(py*cosAP + px*sinAP);
	     //dTDdy -= r0*(x*dxdy + y*dydy);
	     dTDdy -= r0*(x*(k*dAPdy*(px*cosAP - py*sinAP)) + y*(1.f + k*dAPdy*(py*cosAP + px*sinAP)));
	     //dxdpx = k*(sinAP + px*cosAP*dAPdpx - py*sinAP*dAPdpx);
	     //dydpx = k*(py*cosAP*dAPdpx + 1. - cosAP + px*sinAP*dAPdpx);
	     //dTDdpx -= r0*(x*dxdpx + y*dydpx);
	     dTDdpx -= r0*(x*(k*(sinAP + px*cosAP*dAPdpx - py*sinAP*dAPdpx)) + y*(k*(py*cosAP*dAPdpx + 1.f - cosAP + px*sinAP*dAPdpx)));
	     //dxdpy = k*(px*cosAP*dAPdpy - 1. + cosAP - py*sinAP*dAPdpy);
	     //dydpy = k*(sinAP + py*cosAP*dAPdpy + px*sinAP*dAPdpy);
	     //dTDdpy -= r0*(x*dxdpy + y*(dydpy);
	     dTDdpy -= r0*(x*(k*(px*cosAP*dAPdpy - 1.f + cosAP - py*sinAP*dAPdpy)) + y*(k*(sinAP + py*cosAP*dAPdpy + px*sinAP*dAPdpy)));

	  }
	  dprint_np(n, "iteration end, dump parameters" << std::endl
		 << "pos = " << outPar.At(n, 0, 0) << " " << outPar.At(n, 1, 0) << " " << outPar.At(n, 2, 0) << std::endl
		 << "mom = " << outPar.At(n, 3, 0) << " " << outPar.At(n, 4, 0) << " " << outPar.At(n, 5, 0) << std::endl
		 << "r=" << std::sqrt( outPar.At(n, 0, 0)*outPar.At(n, 0, 0) + outPar.At(n, 1, 0)*outPar.At(n, 1, 0) ) << " pT=" << std::sqrt( outPar.At(n, 3, 0)*outPar.At(n, 3, 0) + outPar.At(n, 4, 0)*outPar.At(n, 4, 0) ));
	}
      
      const float TD=totalDistance;
      const float TP=TD*invcurvature;//totalAngPath
      
      dprint_np(n, "TD=" << TD << " TP=" << TP << " arrived at r=" << std::sqrt(outPar.At(n, 0, 0)*outPar.At(n, 0, 0)+outPar.At(n, 1, 0)*outPar.At(n, 1, 0)) << std::endl
        << "pos = " << outPar.At(n, 0, 0) << " " << outPar.At(n, 1, 0) << " " << outPar.At(n, 2, 0) << std::endl
        << "mom = " << outPar.At(n, 3, 0) << " " << outPar.At(n, 4, 0) << " " << outPar.At(n, 5, 0));

      const float iC = invcurvature;
      const float dCdpx = k*pxin*ptinv;
      const float dCdpy = k*pyin*ptinv;
      const float dTPdx = dTDdx*iC;
      const float dTPdy = dTDdy*iC;
      const float dTPdpx = (dTDdpx - TD*dCdpx*iC)*iC; // MT change: avoid division
      const float dTPdpy = (dTDdpy - TD*dCdpy*iC)*iC; // MT change: avoid division

      //std::cout << "dTPdx=" << dTPdx << " dTPdy=" << dTPdy << " dTPdpx=" << dTPdpx << " dTPdpy=" << dTPdpy << std::endl;
      
      float cosTP, sinTP;
      if (Config::useTrigApprox) {
	sincos4(TP, sinTP, cosTP);
      } else {
	cosTP = std::cos(TP);
	sinTP = std::sin(TP);
      }

      dprint_np(n, "sinTP=" << sinTP << " cosTP=" << cosTP << " TD=" << TD);

      //now try to make full jacobian
      //derive these to compute jacobian
      //x = xin + k*(pxin*sinTP-pyin*(1-cosTP));
      //y = yin + k*(pyin*sinTP+pxin*(1-cosTP));
      //z = zin + k*TP*pzin;
      //px = pxin*cosTP-pyin*sinTP;
      //py = pyin*cosTP+pxin*sinTP;
      //pz = pzin;
      //jacobian
      
      errorProp(n,0,0) = 1 + k*dTPdx*(pxin*cosTP - pyin*sinTP);	//dxdx;
      errorProp(n,0,1) = k*dTPdy*(pxin*cosTP - pyin*sinTP);	//dxdy;
      errorProp(n,0,2) = 0.;
      errorProp(n,0,3) = k*(sinTP + pxin*cosTP*dTPdpx - pyin*sinTP*dTPdpx); //dxdpx;
      errorProp(n,0,4) = k*(pxin*cosTP*dTPdpy - 1.f + cosTP - pyin*sinTP*dTPdpy);//dxdpy;
      errorProp(n,0,5) = 0.;
      
      errorProp(n,1,0) = k*dTPdx*(pyin*cosTP + pxin*sinTP);	//dydx;
      errorProp(n,1,1) = 1 + k*dTPdy*(pyin*cosTP + pxin*sinTP);	//dydy;
      errorProp(n,1,2) = 0.;
      errorProp(n,1,3) = k*(pyin*cosTP*dTPdpx + 1.f - cosTP + pxin*sinTP*dTPdpx);//dydpx;
      errorProp(n,1,4) = k*(sinTP + pyin*cosTP*dTPdpy + pxin*sinTP*dTPdpy); //dydpy;
      errorProp(n,1,5) = 0.;
      
      errorProp(n,2,0) = k*pzin*dTPdx;	//dzdx;
      errorProp(n,2,1) = k*pzin*dTPdy;	//dzdy;
      errorProp(n,2,2) = 1.f;
      errorProp(n,2,3) = k*pzin*dTPdpx;//dzdpx;
      errorProp(n,2,4) = k*pzin*dTPdpy;//dzdpy;
      errorProp(n,2,5) = k*TP; //dzdpz;
      
      errorProp(n,3,0) = -dTPdx*(pxin*sinTP + pyin*cosTP);	//dpxdx;
      errorProp(n,3,1) = -dTPdy*(pxin*sinTP + pyin*cosTP);	//dpxdy;
      errorProp(n,3,2) = 0.;
      errorProp(n,3,3) = cosTP - dTPdpx*(pxin*sinTP + pyin*cosTP); //dpxdpx;
      errorProp(n,3,4) = -sinTP - dTPdpy*(pxin*sinTP + pyin*cosTP);//dpxdpy;
      errorProp(n,3,5) = 0.;
      
      errorProp(n,4,0) = -dTPdx*(pyin*sinTP - pxin*cosTP); //dpydx;
      errorProp(n,4,1) = -dTPdy*(pyin*sinTP - pxin*cosTP);	//dpydy;
      errorProp(n,4,2) = 0.;
      errorProp(n,4,3) = +sinTP - dTPdpx*(pyin*sinTP - pxin*cosTP);//dpydpx;
      errorProp(n,4,4) = +cosTP - dTPdpy*(pyin*sinTP - pxin*cosTP);//dpydpy;
      errorProp(n,4,5) = 0.;
      
      errorProp(n,5,0) = 0.;
      errorProp(n,5,1) = 0.;
      errorProp(n,5,2) = 0.;
      errorProp(n,5,3) = 0.;
      errorProp(n,5,4) = 0.;
      errorProp(n,5,5) = 1.f;

#ifdef DEBUG
      if (n < N_proc) {
        dmutex_guard;
        std::cout << n << ": jacobian iterative" << std::endl;
        printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,0,0),errorProp(n,0,1),errorProp(n,0,2),errorProp(n,0,3),errorProp(n,0,4),errorProp(n,0,5));
        printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,1,0),errorProp(n,1,1),errorProp(n,1,2),errorProp(n,1,3),errorProp(n,1,4),errorProp(n,1,5));
        printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,2,0),errorProp(n,2,1),errorProp(n,2,2),errorProp(n,2,3),errorProp(n,2,4),errorProp(n,2,5));
        printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,3,0),errorProp(n,3,1),errorProp(n,3,2),errorProp(n,3,3),errorProp(n,3,4),errorProp(n,3,5));
        printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,4,0),errorProp(n,4,1),errorProp(n,4,2),errorProp(n,4,3),errorProp(n,4,4),errorProp(n,4,5));
        printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,5,0),errorProp(n,5,1),errorProp(n,5,2),errorProp(n,5,3),errorProp(n,5,4),errorProp(n,5,5));
      }
#endif
    }
}

void applyMaterialEffects(const MPlexQF &hitsRl, const MPlexQF& hitsXi,
                                MPlexLS &outErr,       MPlexLV& outPar,
                         const int      N_proc)
{
#pragma simd
  for (int n = 0; n < NN; ++n)
    {
#ifdef CCSCOORD

      float radL = hitsRl.ConstAt(n,0,0);
      if (radL<0.0000000000001f) continue;//ugly, please fixme
      const float x = outPar.ConstAt(n,0,0);
      const float y = outPar.ConstAt(n,0,1);
      const float theta = outPar.ConstAt(n,0,5);
      const float r = std::sqrt(x*x+y*y);
      const float pt = 1.f/outPar.ConstAt(n,0,3);
      const float p = pt/std::sin(theta);
      const float p2 = p*p;
      constexpr float mpi = 0.140; // m=140 MeV, pion
      constexpr float mpi2 = mpi*mpi; // m=140 MeV, pion
      const float beta2 = p2/(p2+mpi2);
      const float beta = std::sqrt(beta2);
      //radiation lenght, corrected for the crossing angle (cos alpha from dot product of radius vector and momentum)
      const float invCos = Config::endcapTest ? 1./std::abs(std::cos(theta)) : p/pt;
      radL = radL * invCos; //fixme works only for barrel geom
      // multiple scattering
      //vary independently phi and theta by the rms of the planar multiple scattering angle
      const float thetaMSC = 0.0136f*std::sqrt(radL)*(1.f+0.038f*std::log(radL))/(beta*p);// eq 32.15
      const float thetaMSC2 = thetaMSC*thetaMSC;
      outErr.At(n, 4, 4) += thetaMSC2;
      outErr.At(n, 5, 5) += thetaMSC2;
      //std::cout << "beta=" << beta << " p=" << p << std::endl;
      //std::cout << "multiple scattering thetaMSC=" << thetaMSC << " thetaMSC2=" << thetaMSC2 << " radL=" << radL << std::endl;
      // energy loss
      const float gamma = 1.f/std::sqrt(1.f - beta2);
      const float gamma2 = gamma*gamma;
      constexpr float me = 0.0005; // m=0.5 MeV, electron
      const float wmax = 2.f*me*beta2*gamma2 / ( 1.f + 2.f*gamma*me/mpi + me*me/(mpi*mpi) );
      constexpr float I = 16.0e-9 * 10.75;
      const float deltahalf = std::log(28.816e-9f * std::sqrt(2.33f*0.498f)/I) + std::log(beta*gamma) - 0.5f;
      const float dEdx = beta<1.f ? (2.f*(hitsXi.ConstAt(n,0,0) * invCos * (0.5f*std::log(2.f*me*beta2*gamma2*wmax/(I*I)) - beta2 - deltahalf) / beta2)) : 0.f;//protect against infs and nans
      // dEdx = dEdx*2.;//xi in cmssw is defined with an extra factor 0.5 with respect to formula 27.1 in pdg
      //std::cout << "dEdx=" << dEdx << " delta=" << deltahalf << " wmax=" << wmax << " Xi=" << hitsXi.ConstAt(n,0,0) << std::endl;
      const float dP = dEdx/beta;
      outPar.At(n, 0, 3) = p/((p+dP)*pt);
      //assume 100% uncertainty
      outErr.At(n, 3, 3) += dP*dP/(p2*pt*pt);
#else
      float radL = hitsRl.ConstAt(n,0,0);
      if (radL<0.0000000000001f) continue;//ugly, please fixme
      const float& x = outPar.ConstAt(n,0,0);
      const float& y = outPar.ConstAt(n,0,1);
      const float& px = outPar.ConstAt(n,0,3);
      const float& py = outPar.ConstAt(n,0,4);
      const float& pz = outPar.ConstAt(n,0,5);
      const float r = std::sqrt(x*x+y*y);
      float pt = px*px + py*py;
      float p2 = pt + pz*pz;
      pt = std::sqrt(pt);
      const float p = std::sqrt(p2);
      constexpr float mpi = 0.140f; // m=140 MeV, pion
      constexpr float mpi2 = mpi*mpi; // m=140 MeV, pion
      const float beta2 = p2/(p2+mpi2);
      const float beta = std::sqrt(beta2);
      //radiation lenght, corrected for the crossing angle (cos alpha from dot product of radius vector and momentum)
      const float invCos = (p*r)/std::abs(x*px+y*py);
      radL = radL * invCos; //fixme works only for barrel geom
      // multiple scattering
      // in a reference frame defined by the orthogonal unit vectors: u=(px/p,py/p,pz/p) v=(-py/pt,px/pt,0) s=(-pzpx/pt/p,-pzpy/pt/p,pt/p)
      // we consider two planar angles theta1 and theta2 in the uv and us planes respectively
      // note theta1 and theta2 are different angles but with the same rms value thetaMSC
      // first order approximation: sin_thetaMSC ~ thetaMSC
      // px' = px + (py*p*theta1 + pz*px*theta2)/pt; 
      // py' = py - (px*p*theta1 - pz*py*theta2)/pt;
      // pz' = pz + pt*theta2;
      // this actually changes |p| so that p'^2 = p^2(1+2thetaMSC^2) so we should renormalize everything but we neglect this effect here (we are just inflating uncertainties a bit)
      const float thetaMSC = 0.0136f*std::sqrt(radL)*(1.f+0.038f*std::log(radL))/(beta*p);// eq 32.15
      const float thetaMSC2 = thetaMSC*thetaMSC;
      const float thetaMSC2overPt2 = thetaMSC2/(pt*pt);
      outErr.At(n, 3, 3) += (py*py*p*p + pz*pz*px*px)*thetaMSC2overPt2;
      outErr.At(n, 4, 4) += (px*px*p*p + pz*pz*py*py)*thetaMSC2overPt2;
      outErr.At(n, 5, 5) += pt*pt*thetaMSC2;
      outErr.At(n, 3, 4) += -px*py*thetaMSC2;
      outErr.At(n, 3, 5) += -pz*px*thetaMSC2;
      outErr.At(n, 4, 5) += -pz*py*thetaMSC2;
      // std::cout << "beta=" << beta << " p=" << p << std::endl;
      // std::cout << "multiple scattering thetaMSC=" << thetaMSC << " thetaMSC2=" << thetaMSC2 << " radL=" << radL << " cxx=" << (py*py*p*p + pz*pz*px*px)*thetaMSC2overPt2 << " cyy=" << (px*px*p*p + pz*pz*py*py)*thetaMSC2overPt2 << " czz=" << pt*pt*thetaMSC2 << std::endl;
      // energy loss
      const float gamma = 1.f/std::sqrt(1.f - beta2);
      const float gamma2 = gamma*gamma;
      constexpr float me = 0.0005; // m=0.5 MeV, electron
      const float wmax = 2.f*me*beta2*gamma2 / ( 1.f + 2.f*gamma*me/mpi + me*me/(mpi*mpi) );
      constexpr float I = 16.0e-9f * 10.75f;
      const float deltahalf = std::log(28.816e-9f * std::sqrt(2.33f*0.498f)/I) + std::log(beta*gamma) - 0.5f;
      const float dEdx = 2.f*(hitsXi.ConstAt(n,0,0) * invCos * (0.5f*std::log(2*me*beta2*gamma2*wmax/(I*I)) - beta2 - deltahalf) / beta2) ;
      //dEdx = dEdx*2.f;//xi in cmssw is defined with an extra factor 0.5 with respect to formula 27.1 in pdg
      // std::cout << "dEdx=" << dEdx << " delta=" << deltahalf << std::endl;
      float dP = dEdx/beta;
      outPar.At(n, 0, 3) -= dP*px/p;
      outPar.At(n, 0, 4) -= dP*py/p;
      outPar.At(n, 0, 5) -= dP*pz/p;
      //assume 100% uncertainty
      dP = dP*dP;//warning, redefining dP!
      p2 = 1.f/p2;//warning, redefining p2!
      outErr.At(n, 3, 3) += dP*px*px*p2;//dP^2*px*px/p^2
      outErr.At(n, 4, 4) += dP*py*py*p2;
      outErr.At(n, 5, 5) += dP*pz*pz*p2;
      p2 = p2/p;//warning, redefining p2!
      outErr.At(n, 3, 4) += dP*px*py*p2;//dP^2*px*py/p^3
      outErr.At(n, 3, 5) += dP*px*pz*p2;
      outErr.At(n, 4, 5) += dP*pz*py*p2;
#endif
    }

}

void propagateHelixToRMPlex(const MPlexLS &inErr,  const MPlexLV& inPar,
                            const MPlexQI &inChg,  const MPlexHV& msPar, 
                            MPlexLS &outErr,       MPlexLV& outPar,
                            const int      N_proc)
{
   const idx_t N  = NN;

   outErr = inErr;
   outPar = inPar;

   MPlexLL errorProp;

   MPlexQF msRad;
   // MPlexQF hitsRl;
   // MPlexQF hitsXi;
#pragma simd
   for (int n = 0; n < NN; ++n) {
     msRad.At(n, 0, 0) = hipo(msPar.ConstAt(n, 0, 0), msPar.ConstAt(n, 1, 0));
     // if (Config::useCMSGeom) {
     //   hitsRl.At(n, 0, 0) = getRlVal(msRad.ConstAt(n, 0, 0), outPar.ConstAt(n, 2, 0));
     //   hitsXi.At(n, 0, 0) = getXiVal(msRad.ConstAt(n, 0, 0), outPar.ConstAt(n, 2, 0));
     // }
   }

#ifdef CCSCOORD
   helixAtRFromIterativeCCS(inPar, inChg, outPar, msRad, errorProp, N_proc);
#else
   helixAtRFromIterative(inPar, inChg, outPar, msRad, errorProp, N_proc);
#endif

#ifdef DEBUG
   {
     for (int kk = 0; kk < N_proc; ++kk)
     {
       dprintf("outErr before prop %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", outErr.At(kk,i,j)); printf("\n");
       } dprintf("\n");

       dprintf("errorProp %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", errorProp.At(kk,i,j)); printf("\n");
       } dprintf("\n");

     }
   }
#endif

   // Matriplex version of:
   // result.errors = ROOT::Math::Similarity(errorProp, outErr);
   MPlexLL temp;
   MultHelixProp      (errorProp, outErr, temp);
   MultHelixPropTransp(errorProp, temp,   outErr);

   // if (Config::useCMSGeom) {
   //   applyMaterialEffects(hitsRl, hitsXi, outErr, outPar);
   // }

   // This dump is now out of its place as similarity is done with matriplex ops.
#ifdef DEBUG
   {
     for (int kk = 0; kk < N_proc; ++kk)
     {
       dprintf("outErr %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", outErr.At(kk,i,j)); printf("\n");
       } dprintf("\n");

       dprintf("outPar %d\n", kk);
       for (int i = 0; i < 6; ++i) {
           dprintf("%8f ", outPar.At(kk,i,0)); printf("\n");
       } dprintf("\n");
       if (std::abs(hipo(outPar.At(kk,0,0), outPar.At(kk,1,0))-hipo(msPar.ConstAt(kk, 0, 0), msPar.ConstAt(kk, 1, 0)))>0.0001) {
	 dprint_np(kk, "DID NOT GET TO R, dR=" << std::abs(hipo(outPar.At(kk,0,0), outPar.At(kk,1,0))-hipo(msPar.ConstAt(kk, 0, 0), msPar.ConstAt(kk, 1, 0)))
		   << " r=" << hipo(msPar.ConstAt(kk, 0, 0), msPar.ConstAt(kk, 1, 0)) << " r0in=" << hipo(inPar.ConstAt(kk,0,0), inPar.ConstAt(kk,1,0)) << " rout=" << hipo(outPar.At(kk,0,0), outPar.At(kk,1,0)) << std::endl
		   << "pt=" << hipo(inPar.ConstAt(kk,3,0), inPar.ConstAt(kk,4,0)) << " pz=" << inPar.ConstAt(kk,5,0));
       }
     }
   }
#endif
}

void propagateHelixToRMPlex(const MPlexLS& inErr,  const MPlexLV& inPar,
                            const MPlexQI& inChg,  const float    r,
                            MPlexLS&       outErr, MPlexLV&       outPar,
                            const int      N_proc)
{
   outErr = inErr;
   outPar = inPar;

   MPlexLL errorProp;


   MPlexQF msRad;
#pragma simd
   for (int n = 0; n < NN; ++n) {
     msRad.At(n, 0, 0) = r;
   }

#ifdef CCSCOORD
   helixAtRFromIterativeCCS(inPar, inChg, outPar, msRad, errorProp, N_proc);
#else
   helixAtRFromIterative(inPar, inChg, outPar, msRad, errorProp, N_proc);
#endif

   //add multiple scattering uncertainty and energy loss (FIXME: in this way it is not applied in track fit)
   if (Config::useCMSGeom) {
     MPlexQF hitsRl;
     MPlexQF hitsXi;
#pragma simd
     for (int n = 0; n < NN; ++n) {
       hitsRl.At(n, 0, 0) = getRlVal(r, outPar.ConstAt(n, 2, 0));
       hitsXi.At(n, 0, 0) = getXiVal(r, outPar.ConstAt(n, 2, 0));
     }
     applyMaterialEffects(hitsRl, hitsXi, outErr, outPar, N_proc);
   }

   // Matriplex version of:
   // result.errors = ROOT::Math::Similarity(errorProp, outErr);

   //MultHelixProp can be optimized for CCS coordinates, see GenMPlexOps.pl
   MPlexLL temp;
   MultHelixProp      (errorProp, outErr, temp);
   MultHelixPropTransp(errorProp, temp,   outErr);

   // This dump is now out of its place as similarity is done with matriplex ops.
#ifdef DEBUG
   if (debug) {
     for (int kk = 0; kk < N_proc; ++kk)
     {
       dprintf("outErr %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", outErr.At(kk,i,j)); printf("\n");
       } dprintf("\n");

       dprintf("outPar %d\n", kk);
       for (int i = 0; i < 6; ++i) {
           dprintf("%8f ", outPar.At(kk,i,0)); printf("\n");
       } dprintf("\n");
     }
   }
#endif

   /*
     if (fabs(sqrt(outPar[0]*outPar[0]+outPar[1]*outPar[1])-r)>0.0001) {
     std::cout << "DID NOT GET TO R, dR=" << fabs(sqrt(outPar[0]*outPar[0]+outPar[1]*outPar[1])-r)
     << " r=" << r << " r0in=" << r0in << " rout=" << sqrt(outPar[0]*outPar[0]+outPar[1]*outPar[1]) << std::endl;
     std::cout << "pt=" << pt << " pz=" << inPar.At(n, 2) << std::endl;
     }
   */
}

void propagateHelixToZMPlex(const MPlexLS &inErr,  const MPlexLV& inPar,
                            const MPlexQI &inChg,  const MPlexHV& msPar,
			          MPlexLS &outErr,       MPlexLV& outPar,
                            const int      N_proc)
{
   const idx_t N  = NN;

   outErr = inErr;
   outPar = inPar;

   MPlexLL errorProp;

   MPlexQF msZ;
   // MPlexQF msRad;
   // MPlexQF hitsRl;
   // MPlexQF hitsXi;
#pragma simd
   for (int n = 0; n < NN; ++n) {
     msZ.At(n, 0, 0) = msPar.ConstAt(n, 2, 0);
     // if (Config::useCMSGeom || Config::readCmsswSeeds) {
     //   msRad.At(n, 0, 0) = hipo(msPar.ConstAt(n, 0, 0), msPar.ConstAt(n, 1, 0));
     //   hitsRl.At(n, 0, 0) = getRlVal(msRad.ConstAt(n, 0, 0), msPar.ConstAt(n, 2, 0));
     //   hitsXi.At(n, 0, 0) = getXiVal(msRad.ConstAt(n, 0, 0), msPar.ConstAt(n, 2, 0));
     // }
   }

   helixAtZ(inPar, inChg, outPar, msZ, errorProp, N_proc);

#ifdef DEBUG
   {
     for (int kk = 0; kk < N_proc; ++kk)
     {
       dprintf("inErr %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", inErr.ConstAt(kk,i,j)); printf("\n");
       } dprintf("\n");

       dprintf("errorProp %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", errorProp.At(kk,i,j)); printf("\n");
       } dprintf("\n");

     }
   }
#endif

   // Matriplex version of:
   // result.errors = ROOT::Math::Similarity(errorProp, outErr);
   MPlexLL temp;
   MultHelixPropEndcap      (errorProp, outErr, temp);
   MultHelixPropTranspEndcap(errorProp, temp,   outErr);

   // if (Config::useCMSGeom || Config::readCmsswSeeds) {
   //   applyMaterialEffects(hitsRl, hitsXi, outErr, outPar, N_proc);
   // }

   // This dump is now out of its place as similarity is done with matriplex ops.
#ifdef DEBUG
   {
     for (int kk = 0; kk < N_proc; ++kk)
     {
       dprintf("outErr %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", outErr.At(kk,i,j)); printf("\n");
       } dprintf("\n");

       dprintf("outPar %d\n", kk);
       for (int i = 0; i < 6; ++i) {
           dprintf("%8f ", outPar.At(kk,i,0)); printf("\n");
       } dprintf("\n");
       if (std::abs(outPar.At(kk,2,0)-msPar.ConstAt(kk, 2, 0))>0.0001) {
	 dprint_np(kk, "DID NOT GET TO Z, dZ=" << std::abs(outPar.At(kk,2,0)-msPar.ConstAt(kk, 2, 0))
		   << " z=" << msPar.ConstAt(kk, 2, 0) << " zin=" << inPar.ConstAt(kk,2,0) << " zout=" << outPar.At(kk,2,0) << std::endl
		   << "pt=" << hipo(inPar.ConstAt(kk,3,0), inPar.ConstAt(kk,4,0)) << " pz=" << inPar.ConstAt(kk,5,0));
       }
     }
   }
#endif
}


void propagateHelixToZMPlex(const MPlexLS &inErr,  const MPlexLV& inPar,
                            const MPlexQI &inChg,  const float z,
			          MPlexLS &outErr,       MPlexLV& outPar,
                            const int      N_proc)
{
   const idx_t N  = NN;

   outErr = inErr;
   outPar = inPar;

   MPlexLL errorProp;

   MPlexQF msZ;
#pragma simd
   for (int n = 0; n < NN; ++n) {
     msZ.At(n, 0, 0) = (inPar.ConstAt(n, 2, 0) > 0) ? z : -z;
   }

   helixAtZ(inPar, inChg, outPar, msZ, errorProp, N_proc);

     MPlexQF msRad;
     MPlexQF hitsRl;
     MPlexQF hitsXi;
     if (Config::useCMSGeom || Config::readCmsswSeeds) {
#pragma simd
       for (int n = 0; n < NN; ++n) {
	 msRad.At(n, 0, 0) = hipo(outPar.ConstAt(n, 0, 0), outPar.ConstAt(n, 1, 0));
	 hitsRl.At(n, 0, 0) = getRlVal(msRad.ConstAt(n, 0, 0), z);
	 hitsXi.At(n, 0, 0) = getXiVal(msRad.ConstAt(n, 0, 0), z);
       }
     }

#ifdef DEBUG
   {
     for (int kk = 0; kk < N_proc; ++kk)
     {
       dprintf("inErr %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", inErr.ConstAt(kk,i,j)); printf("\n");
       } dprintf("\n");

       dprintf("errorProp %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", errorProp.At(kk,i,j)); printf("\n");
       } dprintf("\n");

     }
   }
#endif

   // Matriplex version of:
   // result.errors = ROOT::Math::Similarity(errorProp, outErr);
   MPlexLL temp;
   MultHelixPropEndcap      (errorProp, outErr, temp);
   MultHelixPropTranspEndcap(errorProp, temp,   outErr);

   if (Config::useCMSGeom || Config::readCmsswSeeds) {
     applyMaterialEffects(hitsRl, hitsXi, outErr, outPar, N_proc);
   }

   // This dump is now out of its place as similarity is done with matriplex ops.
#ifdef DEBUG
   {
     for (int kk = 0; kk < N_proc; ++kk)
     {
       dprintf("outErr %d\n", kk);
       for (int i = 0; i < 6; ++i) { for (int j = 0; j < 6; ++j)
           dprintf("%8f ", outErr.At(kk,i,j)); printf("\n");
       } dprintf("\n");

       dprintf("outPar %d\n", kk);
       for (int i = 0; i < 6; ++i) {
           dprintf("%8f ", outPar.At(kk,i,0)); printf("\n");
       } dprintf("\n");
       if (std::abs(outPar.At(kk,2,0)-msPar.ConstAt(kk, 2, 0))>0.0001) {
	 dprint_np(kk, "DID NOT GET TO Z, dZ=" << std::abs(outPar.At(kk,2,0)-msPar.ConstAt(kk, 2, 0))
		   << " z=" << msPar.ConstAt(kk, 2, 0) << " zin=" << inPar.ConstAt(kk,2,0) << " zout=" << outPar.At(kk,2,0) << std::endl
		   << "pt=" << hipo(inPar.ConstAt(kk,3,0), inPar.ConstAt(kk,4,0)) << " pz=" << inPar.ConstAt(kk,5,0));
       }
     }
   }
#endif
}


void helixAtZ(const MPlexLV& inPar, const MPlexQI& inChg, MPlexLV& outPar,
	      const MPlexQF &msZ, MPlexLL& errorProp,
	      const int      N_proc)
{
  errorProp.SetVal(0.f);

#pragma simd
  for (int n = 0; n < NN; ++n)
    {
      //initialize erroProp to identity matrix, except element 2,2 which is zero
      errorProp(n,0,0) = 1.f;
      errorProp(n,1,1) = 1.f;
      errorProp(n,3,3) = 1.f;
      errorProp(n,4,4) = 1.f;
      errorProp(n,5,5) = 1.f;

      const float k = inChg.ConstAt(n, 0, 0) * 100.f / (-Config::sol*Config::Bfield);
      const float zout = msZ.ConstAt(n, 0, 0);

      const float xin   = inPar.ConstAt(n, 0, 0);
      const float yin   = inPar.ConstAt(n, 1, 0);
      const float zin   = inPar.ConstAt(n, 2, 0);
      const float ipt   = inPar.ConstAt(n, 3, 0);
      const float phiin = inPar.ConstAt(n, 4, 0);
      const float theta = inPar.ConstAt(n, 5, 0);

      dprint_np(n, std::endl << "input parameters"
            << " inPar.ConstAt(n, 0, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 0, 0)
            << " inPar.ConstAt(n, 1, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 1, 0)
            << " inPar.ConstAt(n, 2, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 2, 0)
            << " inPar.ConstAt(n, 3, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 3, 0)
            << " inPar.ConstAt(n, 4, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 4, 0)
            << " inPar.ConstAt(n, 5, 0)=" << std::setprecision(9) << inPar.ConstAt(n, 5, 0)
            );

      const float kinv  = 1.f/k;
      const float pt = 1.f/ipt;

      float D = 0., cosa = 0., sina = 0., id = 0.;
      //no trig approx here, phi can be large
      float cosP = std::cos(phiin), sinP = std::sin(phiin);
      float cosT = std::cos(theta), sinT = std::sin(theta);
      const float pxin = cosP*pt;
      const float pyin = sinP*pt;

      //fixme, make this printout useful for propagation to z
      dprint_np(n, std::endl << "k=" << std::setprecision(9) << k << " pxin=" << std::setprecision(9) << pxin << " pyin="
             << std::setprecision(9) << pyin << " cosP=" << std::setprecision(9) << cosP
             << " sinP=" << std::setprecision(9) << sinP << " pt=" << std::setprecision(9) << pt);

      const float deltaZ = zout - zin;
      const float alpha  = deltaZ*sinT*ipt/(cosT*k);

      if (Config::useTrigApprox) {
	sincos4(alpha, sina, cosa);
      } else {
	cosa=std::cos(alpha);
	sina=std::sin(alpha);
      }

      //update parameters
      outPar.At(n, 0, 0) = outPar.At(n, 0, 0) + k*(pxin*sina - pyin*(1.f-cosa));
      outPar.At(n, 1, 0) = outPar.At(n, 1, 0) + k*(pyin*sina + pxin*(1.f-cosa));
      outPar.At(n, 2, 0) = zout;
      outPar.At(n, 4, 0) = phiin+alpha;

      dprint_np(n, std::endl << "outPar.At(n, 0, 0)=" << outPar.At(n, 0, 0) << " outPar.At(n, 1, 0)=" << outPar.At(n, 1, 0)
		<< " pxin=" << pxin << " pyin=" << pyin);

      errorProp(n,0,2) = cosP*sinT*(sinP*cosa*sin(cosP*sina) - cosa)/cosT;
      errorProp(n,0,3) = cosP*sinT*deltaZ*cosa*( 1.f - sinP*sin(cosP*sina) )/(cosT*ipt) - k*(cosP*sina - sinP*(1.f-cos(cosP*sina)))/(ipt*ipt);
      errorProp(n,0,4) = (k/ipt)*( -sinP*sina + sinP*sinP*sina*sin(cosP*sina) - cosP*(1.f - cos(cosP*sina) ) );
      errorProp(n,0,5) = cosP*deltaZ*cosa*( 1.f - sinP*sin(cosP*sina) )/(cosT*cosT);

      errorProp(n,1,2) = cosa*sinT*(cosP*cosP*sin(cosP*sina) - sinP)/cosT;
      errorProp(n,1,3) = sinT*deltaZ*cosa*( cosP*cosP*sin(cosP*sina) + sinP )/(cosT*ipt) - k*(sinP*sina + cosP*(1.f-cos(cosP*sina)))/(ipt*ipt);
      errorProp(n,1,4) = (k/ipt)*( -sinP*(1.f - cos(cosP*sina)) - sinP*cosP*sina*sin(cosP*sina) + cosP*sina );
      errorProp(n,1,5) = deltaZ*cosa*( cosP*cosP*sin(cosP*sina) + sinP )/(cosT*cosT);

      errorProp(n,4,2) = -ipt*sinT/(cosT*k);
      errorProp(n,4,3) = sinT*deltaZ/(cosT*k);
      errorProp(n,4,5) = ipt*deltaZ/(cosT*cosT*k);

      dprint_np(n, "propagation end, dump parameters" << std::endl
	     << "pos = " << outPar.At(n, 0, 0) << " " << outPar.At(n, 1, 0) << " " << outPar.At(n, 2, 0) << std::endl
	     << "mom = " << std::cos(outPar.At(n, 4, 0))/outPar.At(n, 3, 0) << " " << std::sin(outPar.At(n, 4, 0))/outPar.At(n, 3, 0) << " " << 1./(outPar.At(n, 3, 0)*tan(outPar.At(n, 5, 0)))
	     << " r=" << std::sqrt( outPar.At(n, 0, 0)*outPar.At(n, 0, 0) + outPar.At(n, 1, 0)*outPar.At(n, 1, 0) ) << " pT=" << 1./std::abs(outPar.At(n, 3, 0)) << std::endl);

#ifdef DEBUG
      if (n < N_proc) {
	dmutex_guard;
	std::cout << n << ": jacobian" << std::endl;
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,0,0),errorProp(n,0,1),errorProp(n,0,2),errorProp(n,0,3),errorProp(n,0,4),errorProp(n,0,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,1,0),errorProp(n,1,1),errorProp(n,1,2),errorProp(n,1,3),errorProp(n,1,4),errorProp(n,1,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,2,0),errorProp(n,2,1),errorProp(n,2,2),errorProp(n,2,3),errorProp(n,2,4),errorProp(n,2,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,3,0),errorProp(n,3,1),errorProp(n,3,2),errorProp(n,3,3),errorProp(n,3,4),errorProp(n,3,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,4,0),errorProp(n,4,1),errorProp(n,4,2),errorProp(n,4,3),errorProp(n,4,4),errorProp(n,4,5));
	printf("%5f %5f %5f %5f %5f %5f\n", errorProp(n,5,0),errorProp(n,5,1),errorProp(n,5,2),errorProp(n,5,3),errorProp(n,5,4),errorProp(n,5,5));
      }
#endif
    }
}
