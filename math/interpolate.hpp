#ifndef MMATH_INTERPOLATE_H
#define MMATH_INTERPOLATE_H
#include <cmath>

namespace Math {
  namespace Interpolator {

    /** Interpolate between values - creating data out of nothing.
     *
     * The math mainly comes from:
     * http://local.wasp.uwa.edu.au/~pbourke/other/interpolation/
     *
     * Capsuled into classes which store the values to allow usage
     * as a kind of data - consumer/producer.
     *
     * This idea comes from here: */
     // http://exult.cvs.sourceforge.net/*checkout*/exult/exult/audio/Audio.cc

    template <typename T>
      class Linear {
        public:
          Linear(const T & y1, const T & y2);
          T    getAt(const T & mu) const;
          void shiftAndFeed(const T & y_i);
        private:
          T v1, v2;
      };

    template <typename T>
      class Cubic {
        public:
          Cubic(const T & y0,
              const T & y1, const T & y2, const T & y3);
          Cubic(const T & y1,
              const T & y2, const T & y3);

          void  shiftAndFeed(const T & y_i);
          void  shift();
          T     getAt(const T & mu) const;

        private:
          void  setCoefficients(const T y0,
              const T y1, const T y2, const T y3);
          T a0, a1, a2, a3;
          T v2, v3;
      };

    template <typename T>
      class Cosine {
        public:
          Cosine(const T & y1, const T & y2);

          T    getAt(const T & mu) const;
          void shiftAndFeed(const T & y_i);

        private:
          T v1, v2;
      };

    template <typename T>
      class Hermite {
        public:
          Hermite(const T & y1, const T & y2, const T & y3,
            const T & _tension, const T & _bias);
          Hermite(const T & y0, const T & y1, const T & y2,
            const T & y3,  const T & _tension, const T & _bias);

          T    getAt(const T & mu) const;
          void shiftAndFeed(const T & y_i);
          void shift();
          void setTension(const T & _tension);
          void setBias(const T & _bias);

        private:
          void setCoefficients(const T y0, const T y1, const T y2,
            const T y3);
          void updateCoefficients();
          T m0, m1;
          T v0, v1, v2, v3;
          T tension, bias;
      };

    // implementation

    template <typename T>
      Linear<T>::Linear(const T & y1, const T & y2) : v1(y1), v2(y2) {}

    template <typename T>
      T Linear<T>::getAt(const T & mu) const {
        return v1 * (1 - mu) + v2 * mu;
      }

    template <typename T>
      void Linear<T>::shiftAndFeed(const T & y_i) {
        v1 = v2;
        v2 = y_i;
      }

    // Cubic

    template <typename T>
      Cubic<T>::Cubic(const T & y0,
          const T & y1, const T & y2, const T & y3) {
        setCoefficients(y0, y1, y2, y3);
      }

    template <typename T>
      Cubic<T>::Cubic(const T & y1,
          const T & y2, const T & y3) {
        setCoefficients(2 * y1 - y2, y1, y2, y3);
      }

    template <typename T>
      void Cubic<T>::shiftAndFeed(const T & y_i) {
        setCoefficients(a3, v2, v3, y_i);  
      }

    template <typename T>
      void Cubic<T>::shift() {
        setCoefficients(a3, v2, v3, 2 * v2 - a3);  
      }

    template <typename T>
      void Cubic<T>::setCoefficients(const T y0,
          const T y1, const T y2, const T y3) {
        v2 = y2; v3 = y3;

        a0 = y3 - y2 - y0 + y1;
        a1 = y0 - y1 - a0;
        a2 = y2 - y0;
        a3 = y1;
      }

    template <typename T>
      T Cubic<T>::getAt(const T & mu) const {
        const T mu_sq = mu * mu;
        return(a0 * mu * mu_sq + a1 * mu_sq + a2 * mu + a3);
      }

    // Cosine

    template <typename T>
      Cosine<T>::Cosine(const T & y1, const T & y2) : v1(y1), v2(y2) {}

    template <typename T>
      T Cosine<T>::getAt(const T & mu) const {
        T mu2 = (1 - cos(mu * M_PI)) / 2;
        return v1 * (1 - mu2) + v2 * mu2;
      }

    template <typename T>
      void Cosine<T>::shiftAndFeed(const T & y_i) {
        v1 = v2;
        v2 = y_i;
      }

    // Hermite

    template <typename T>
      Hermite<T>::Hermite(const T & y1, const T & y2, const T & y3,
          const T & _tension, const T & _bias) : tension(_tension), bias(_bias) {
        setCoefficients(2 * y1 - y2, y1, y2, y3);
      }

    template <typename T>
      T Hermite<T>::getAt(const T & mu) const {
        const T mu_sq = mu * mu;
        const T mu_cu = mu_sq * mu;
        
        T a0 = 2 * mu_cu - 3 * mu_sq + 1;
        T a1 =     mu_cu - 2 * mu_sq + mu;
        T a2 =     mu_cu -     mu_sq;
        T a3 =-2 * mu_cu + 3 * mu_sq;

        return a0 * v1 + a1 * m0 + a2 * m1 + a3 * v2;
      }

    template <typename T>
      void Hermite<T>::setCoefficients(const T y0, const T y1, const T y2,
          const T y3) {
          v0 = y0; v1 = y1; v2 = y2; v3 = y3;
          updateCoefficients();
      }

    template <typename T>
      void Hermite<T>::updateCoefficients() {
          m0  = (v1 - v0) * (1 + bias) * (1 - tension) / 2;
          m0 += (v2 - v1) * (1 - bias) * (1 - tension) / 2;
          m1  = (v2 - v1) * (1 + bias) * (1 - tension) / 2;
          m1 += (v3 - v2) * (1 - bias) * (1 - tension) / 2;
      }

    template <typename T>
      void Hermite<T>::setTension(const T & _tension) {
        tension = _tension;
        updateCoefficients();
      }

    template <typename T>
      void Hermite<T>::setBias(const T & _bias) {
        bias = _bias;
        updateCoefficients();
      }

    template <typename T>
      void Hermite<T>::shiftAndFeed(const T & y_i) {
        setCoefficients(v1, v2, v3, y_i);
      }

    template <typename T>
      void Hermite<T>::shift() {
        setCoefficients(v1, v2, v3, 2 * v2 - v1);
      }

  }
}

#endif
