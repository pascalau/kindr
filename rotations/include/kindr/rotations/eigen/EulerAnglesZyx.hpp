/*
 * Copyright (c) 2013, Christian Gehring, Hannes Sommer, Paul Furgale, Remo Diethelm
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Autonomous Systems Lab, ETH Zurich nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Christian Gehring, Hannes Sommer, Paul Furgale,
 * Remo Diethelm BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#ifndef KINDR_ROTATIONS_EIGEN_EULERANGLESZYX_HPP_
#define KINDR_ROTATIONS_EIGEN_EULERANGLESZYX_HPP_

#include <cmath>

#include <Eigen/Geometry>

#include "kindr/common/common.hpp"
#include "kindr/common/assert_macros_eigen.hpp"
#include "kindr/rotations/RotationBase.hpp"
#include "kindr/rotations/eigen/RotationEigenFunctions.hpp"

namespace kindr {
namespace rotations {
namespace eigen_impl {


/*! \class EulerAnglesZyx
 *  \brief Implementation of Euler angles (Z-Y'-X'' / yaw-pitch-roll) rotation based on Eigen::Matrix<Scalar, 3, 1>
 *
 *  The following typedefs are provided for convenience:
 *   - \ref eigen_impl::EulerAnglesZyxAD "EulerAnglesZyxAD" for active rotation and double primitive type
 *   - \ref eigen_impl::EulerAnglesZyxAF "EulerAnglesZyxAF" for active rotation and float primitive type
 *   - \ref eigen_impl::EulerAnglesZyxPD "EulerAnglesZyxPD" for passive rotation and double primitive type
 *   - \ref eigen_impl::EulerAnglesZyxPF "EulerAnglesZyxPF" for passive rotation and float primitive type
 *   - EulerAnglesYprAD = EulerAnglesZyxAD
 *   - EulerAnglesYprAF = EulerAnglesZyxAF
 *   - EulerAnglesYprPD = EulerAnglesZyxPD
 *   - EulerAnglesYprPF = EulerAnglesZyxPF
 *
 *  \tparam PrimType_ the primitive type of the data (double or float)
 *  \tparam Usage_ the rotation usage which is either active or passive
 *  \ingroup rotations
 */
template<typename PrimType_, enum RotationUsage Usage_>
class EulerAnglesZyx : public EulerAnglesZyxBase<EulerAnglesZyx<PrimType_, Usage_>, Usage_> {
 private:
  /*! \brief The base type.
   */
  typedef Eigen::Matrix<PrimType_, 3, 1> Base;

  /*! \brief vector of Euler angles [yaw; pitch; roll]
   */
  Base zyx_;
 public:
  /*! \brief The implementation type.
   *  The implementation type is always an Eigen object.
   */
  typedef Base Implementation;
  /*! \brief The primitive type.
   *  Float/Double
   */
  typedef PrimType_ Scalar;

  /*! \brief Default constructor using identity rotation.
   */
  EulerAnglesZyx()
    : zyx_(Base::Zero()) {
  }

  /*! \brief Constructor using three scalars.
   *  \param yaw      first rotation angle around Z axis
   *  \param pitch    second rotation angle around Y' axis
   *  \param roll     third rotation angle around X'' axis
   */
  EulerAnglesZyx(Scalar yaw, Scalar pitch, Scalar roll) {
    if(Usage_ == RotationUsage::ACTIVE) {
      zyx_ << yaw,pitch,roll;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      zyx_<< -yaw,-pitch,-roll;
    }
  }

  /*! \brief Constructor using Eigen::Matrix.
   *  \param other   Eigen::Matrix<PrimType_,3,1> [roll; pitch; yaw]
   */
  explicit EulerAnglesZyx(const Base& other) {
    if(Usage_ == RotationUsage::ACTIVE) {
      zyx_ = other;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      zyx_ = -other;
    }
  }

  /*! \brief Constructor using another rotation.
   *  \param other   other rotation
   */
  template<typename OtherDerived_>
  inline explicit EulerAnglesZyx(const RotationBase<OtherDerived_, Usage_>& other)
    : zyx_(internal::ConversionTraits<EulerAnglesZyx, OtherDerived_>::convert(other.derived()).toStoredImplementation()) {
  }

  /*! \brief Assignment operator using another rotation.
   *  \param other   other rotation
   *  \returns referece
   */
  template<typename OtherDerived_>
  EulerAnglesZyx& operator =(const RotationBase<OtherDerived_, Usage_>& other) {
    *this = internal::ConversionTraits<EulerAnglesZyx, OtherDerived_>::convert(other.derived());
    return *this;
  }


  /*! \brief Parenthesis operator to convert from another rotation.
   *  \param other   other rotation
   *  \returns reference
   */
  template<typename OtherDerived_>
  EulerAnglesZyx& operator ()(const RotationBase<OtherDerived_, Usage_>& other) {
    *this = internal::ConversionTraits<EulerAnglesZyx, OtherDerived_>::convert(other.derived());
    return *this;
  }

  /*! \brief Returns the inverse of the rotation.
   *  \returns the inverse of the rotation
   */
  EulerAnglesZyx inverted() const {
    return EulerAnglesZyx(getInverseRpy<PrimType_, PrimType_>(zyx_));
  }

  /*! \brief Inverts the rotation.
   *  \returns reference
   */
  EulerAnglesZyx& inverted() {
    *this = EulerAnglesZyx(getInverseRpy<PrimType_, PrimType_>(zyx_));
    return *this;
  }

  /*! \brief Returns the type used for the implementation.
   *  \returns the type used for the implementation
   */
  Implementation toImplementation() const {
    if(Usage_ == RotationUsage::ACTIVE) {
      return zyx_;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      return -zyx_;
    }
  }

  /*! \brief Cast to the implementation type.
   *  \returns the implementation for direct manipulation (recommended only for advanced users)
   */
  inline Base& toStoredImplementation() {
    return static_cast<Base&>(zyx_);
  }

  /*! \brief Cast to the implementation type.
   *  \returns the implementation for direct manipulation (recommended only for advanced users)
   */
  inline const Base& toStoredImplementation() const {
    return static_cast<const Base&>(zyx_);
  }

  /*! \brief Gets yaw (Z) angle.
   *  \returns yaw angle (scalar)
   */
  inline Scalar yaw() const {
    if(Usage_ == RotationUsage::ACTIVE) {
      return zyx_(0);
    } else if(Usage_ == RotationUsage::PASSIVE) {
      return -zyx_(0);
    }
  }

  /*! \brief Gets pitch (Y') angle.
   *  \returns pitch angle (scalar)
   */
  inline Scalar pitch() const {
    if(Usage_ == RotationUsage::ACTIVE) {
      return zyx_(1);
    } else if(Usage_ == RotationUsage::PASSIVE) {
      return -zyx_(1);
    }
  }

  /*! \brief Gets roll (X'') angle.
   *  \returns roll angle (scalar)
   */
  inline Scalar roll() const {
    if(Usage_ == RotationUsage::ACTIVE) {
      return zyx_(2);
    } else if(Usage_ == RotationUsage::PASSIVE) {
      return -zyx_(2);
    }
  }

  /*! \brief Sets yaw (Z) angle.
   */
  inline void setYaw(Scalar yaw) {
    if(Usage_ == RotationUsage::ACTIVE) {
      zyx_(0) = yaw;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      zyx_(0) = -yaw;
    }
  }

  /*! \brief Sets pitch (Y') angle.
   */
  inline void setPitch(Scalar pitch) {
    if(Usage_ == RotationUsage::ACTIVE) {
      zyx_(1) = pitch;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      zyx_(1) = -pitch;
    }
  }

  /*! \brief Sets roll (X'') angle.
   */
  inline void setRoll(Scalar roll) {
    if(Usage_ == RotationUsage::ACTIVE) {
      zyx_(2) = roll;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      zyx_(2) = -roll;
    }
  }

  /*! \brief Reading access to yaw (Z) angle.
   *  \returns yaw angle (scalar) with reading access
   */
  inline Scalar z() const {
    if(Usage_ == RotationUsage::ACTIVE) {
      return zyx_(0);
    } else if(Usage_ == RotationUsage::PASSIVE) {
      return -zyx_(0);
    }
  }

  /*! \brief Reading access to pitch (Y') angle.
   *  \returns pitch angle (scalar) with reading access
   */
  inline Scalar y() const {
    if(Usage_ == RotationUsage::ACTIVE) {
      return zyx_(1);
    } else if(Usage_ == RotationUsage::PASSIVE) {
      return -zyx_(1);
    }
  }

  /*! \brief Reading access to roll (X'') angle.
   *  \returns roll angle (scalar) with reading access
   */
  inline Scalar x() const {
    if(Usage_ == RotationUsage::ACTIVE) {
      return zyx_(2);
    } else if(Usage_ == RotationUsage::PASSIVE) {
      return -zyx_(2);
    }
  }

  /*! \brief Writing access to yaw (Z) angle.
   *  \returns yaw angle (scalar) with writing access
   */
  inline void setZ(Scalar z) {
    if(Usage_ == RotationUsage::ACTIVE) {
      zyx_(0) = z;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      zyx_(0) = -z;
    }
  }

  /*! \brief Writing access to pitch (Y') angle.
   *  \returns pitch angle (scalar) with writing access
   */
  inline void setY(Scalar y) {
    if(Usage_ == RotationUsage::ACTIVE) {
      zyx_(1) = y;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      zyx_(1) = -y;
    }
  }

  /*! \brief Writing access to roll (X'') angle.
   *  \returns roll angle (scalar) with writing access
   */
  inline void setX(Scalar x) {
    if(Usage_ == RotationUsage::ACTIVE) {
      zyx_(2) = x;
    } else if(Usage_ == RotationUsage::PASSIVE) {
      zyx_(2) = -x;
    }
  }

  /*! \brief Sets the rotation to identity.
   *  \returns reference
   */
  EulerAnglesZyx& setIdentity() {
    this->Implementation::setZero();
    return *this;
  }

  /*! \brief Returns a unique Euler angles rotation with angles in [-pi,pi),[-pi/2,pi/2),[-pi,pi).
   *  This function is used to compare different rotations.
   *  \returns copy of the Euler angles rotation which is unique
   */
  EulerAnglesZyx getUnique() const {  // wraps angles into [-pi,pi),[-pi/2,pi/2),[-pi,pi)
    EulerAnglesZyx zyx(kindr::common::floatingPointModulo(yaw()  +M_PI,2*M_PI)-M_PI,
                       kindr::common::floatingPointModulo(pitch()+M_PI,2*M_PI)-M_PI,
                       kindr::common::floatingPointModulo(roll() +M_PI,2*M_PI)-M_PI); // wraps all angles into [-pi,pi)
    if(zyx.pitch() >= M_PI/2)
    {
      if(zyx.yaw() >= 0) {
        zyx.setYaw(zyx.yaw() - M_PI);
      } else {
        zyx.setYaw(zyx.yaw() + M_PI);
      }

      zyx.setPitch(-(zyx.pitch() - M_PI));

      if(zyx.roll() >= 0) {
        zyx.setRoll(zyx.roll() - M_PI);
      } else {
        zyx.setRoll(zyx.roll() + M_PI);
      }
    }
    else if(zyx.pitch() < -M_PI/2)
    {
      if(zyx.yaw() >= 0) {
        zyx.setYaw(zyx.yaw() - M_PI);
      } else {
        zyx.setYaw(zyx.yaw() + M_PI);
      }

      zyx.setPitch(-(zyx.pitch() + M_PI));

      if(zyx.roll() >= 0) {
        zyx.setRoll(zyx.roll() - M_PI);
      } else {
        zyx.setRoll(zyx.roll() + M_PI);
      }
    }
    return zyx;
  }

  /*! \brief Modifies the Euler angles rotation such that the angles lie in [-pi,pi),[-pi/2,pi/2),[-pi,pi).
   *  \returns reference
   */
  EulerAnglesZyx& setUnique() {  // wraps angles into [-pi,pi),[-pi/2,pi/2),[-pi,pi)
    *this = getUnique();
    return *this;
  }

  /*! \brief Concenation operator.
   *  This is explicitly specified, because Eigen::Matrix provides also an operator*.
   *  \returns the concenation of two rotations
   */
  using EulerAnglesZyxBase<EulerAnglesZyx<PrimType_, Usage_>, Usage_>::operator*;

  /*! \brief Equivalence operator.
   *  This is explicitly specified, because Eigen::Matrix provides also an operator==.
   *  \returns true if two rotations are similar.
   */
  using EulerAnglesZyxBase<EulerAnglesZyx<PrimType_, Usage_>, Usage_>::operator==;

  /*! \brief Used for printing the object with std::cout.
   *  \returns std::stream object
   */
  friend std::ostream& operator << (std::ostream& out, const EulerAnglesZyx& zyx) {
    out << zyx.toImplementation().transpose();
    return out;
  }
};

//! \brief Active Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) with double primitive type
typedef EulerAnglesZyx<double, RotationUsage::ACTIVE>  EulerAnglesZyxAD;
//! \brief Active Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) with float primitive type
typedef EulerAnglesZyx<float,  RotationUsage::ACTIVE>  EulerAnglesZyxAF;
//! \brief Passive Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) with double primitive type
typedef EulerAnglesZyx<double, RotationUsage::PASSIVE> EulerAnglesZyxPD;
//! \brief Passive Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) with float primitive type
typedef EulerAnglesZyx<float,  RotationUsage::PASSIVE> EulerAnglesZyxPF;

//! \brief Equivalent Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) class
template <typename PrimType_, enum RotationUsage Usage_>
using EulerAnglesYpr = EulerAnglesZyx<PrimType_, Usage_>;

//! \brief Active Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) with double primitive type
typedef EulerAnglesYpr<double, RotationUsage::ACTIVE>  EulerAnglesYprAD;
//! \brief Active Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) with float primitive type
typedef EulerAnglesYpr<float,  RotationUsage::ACTIVE>  EulerAnglesYprAF;
//! \brief Passive Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) with double primitive type
typedef EulerAnglesYpr<double, RotationUsage::PASSIVE> EulerAnglesYprPD;
//! \brief Passive Euler angles rotation (Z,Y',X'' / yaw,pitch,roll) with float primitive type
typedef EulerAnglesYpr<float,  RotationUsage::PASSIVE> EulerAnglesYprPF;

} // namespace eigen_impl


namespace internal {

template<typename PrimType_, enum RotationUsage Usage_>
class get_matrix3X<eigen_impl::EulerAnglesZyx<PrimType_, Usage_>>{
 public:
  typedef int  IndexType;

  template <IndexType Cols>
  using Matrix3X = Eigen::Matrix<PrimType_, 3, Cols>;
};

template<typename PrimType_>
class get_other_usage<eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::ACTIVE>> {
 public:
  typedef eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::PASSIVE> OtherUsage;
};

template<typename PrimType_>
class get_other_usage<eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::PASSIVE>> {
 public:
  typedef eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::ACTIVE> OtherUsage;
};

/* -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Conversion Traits
 * ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
template<typename DestPrimType_, typename SourcePrimType_, enum RotationUsage Usage_>
class ConversionTraits<eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>, eigen_impl::AngleAxis<SourcePrimType_, Usage_>> {
 public:
  inline static eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_> convert(const eigen_impl::AngleAxis<SourcePrimType_, Usage_>& aa) {
    return eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>(eigen_impl::getYprFromAngleAxis<SourcePrimType_, DestPrimType_>(aa.toStoredImplementation()));
  }
};

template<typename DestPrimType_, typename SourcePrimType_, enum RotationUsage Usage_>
class ConversionTraits<eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>, eigen_impl::RotationVector<SourcePrimType_, Usage_>> {
 public:
  inline static eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_> convert(const eigen_impl::RotationVector<SourcePrimType_, Usage_>& rv) {
    return eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>(eigen_impl::getYprFromAngleAxis<SourcePrimType_, DestPrimType_>(eigen_impl::AngleAxis<SourcePrimType_, Usage_>(rv.toStoredImplementation().norm(), rv.toStoredImplementation().normalized()).toStoredImplementation()));
  }
};

template<typename DestPrimType_, typename SourcePrimType_, enum RotationUsage Usage_>
class ConversionTraits<eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>, eigen_impl::RotationQuaternion<SourcePrimType_, Usage_>> {
 public:
  inline static eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_> convert(const eigen_impl::RotationQuaternion<SourcePrimType_, Usage_>& q) {
    return eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>(eigen_impl::getYprFromQuaternion<SourcePrimType_, DestPrimType_>(q.toStoredImplementation()));
  }
};

template<typename DestPrimType_, typename SourcePrimType_, enum RotationUsage Usage_>
class ConversionTraits<eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>, eigen_impl::RotationMatrix<SourcePrimType_, Usage_>> {
 public:
  inline static eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_> convert(const eigen_impl::RotationMatrix<SourcePrimType_, Usage_>& R) {
    return eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>(eigen_impl::getYprFromRotationMatrix<SourcePrimType_, DestPrimType_>(R.toStoredImplementation()));
  }
};

template<typename DestPrimType_, typename SourcePrimType_, enum RotationUsage Usage_>
class ConversionTraits<eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>, eigen_impl::EulerAnglesXyz<SourcePrimType_, Usage_>> {
 public:
  inline static eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_> convert(const eigen_impl::EulerAnglesXyz<SourcePrimType_, Usage_>& xyz) {
    return eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>(eigen_impl::getYprFromRpy<SourcePrimType_, DestPrimType_>(xyz.toStoredImplementation()));
  }
};

template<typename DestPrimType_, typename SourcePrimType_, enum RotationUsage Usage_>
class ConversionTraits<eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>, eigen_impl::EulerAnglesZyx<SourcePrimType_, Usage_>> {
 public:
  inline static eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_> convert(const eigen_impl::EulerAnglesZyx<SourcePrimType_, Usage_>& zyx) {
    return eigen_impl::EulerAnglesZyx<DestPrimType_, Usage_>(zyx.toStoredImplementation().template cast<DestPrimType_>());
  }
};

/* -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Multiplication Traits
 * ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
template<typename PrimType_>
class MultiplicationTraits<RotationBase<eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::ACTIVE>, RotationUsage::ACTIVE>, RotationBase<eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::ACTIVE>, RotationUsage::ACTIVE>> {
 public:
  inline static eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::ACTIVE> mult(const eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::ACTIVE>& a, const eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::ACTIVE>& b) {
    return eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::ACTIVE>(eigen_impl::RotationQuaternion<PrimType_, RotationUsage::ACTIVE>(
                                                                 eigen_impl::RotationQuaternion<PrimType_, RotationUsage::ACTIVE>(a).toStoredImplementation()*
                                                                 eigen_impl::RotationQuaternion<PrimType_, RotationUsage::ACTIVE>(b).toStoredImplementation()));
  }
};

template<typename PrimType_>
class MultiplicationTraits<RotationBase<eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::PASSIVE>, RotationUsage::PASSIVE>, RotationBase<eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::PASSIVE>, RotationUsage::PASSIVE>> {
 public:
  inline static eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::PASSIVE> mult(const eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::PASSIVE>& a, const eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::PASSIVE>& b) {
    return eigen_impl::EulerAnglesZyx<PrimType_, RotationUsage::PASSIVE>(eigen_impl::RotationQuaternion<PrimType_, RotationUsage::PASSIVE>(
                                                                 eigen_impl::RotationQuaternion<PrimType_, RotationUsage::PASSIVE>(a).toStoredImplementation()*
                                                                 eigen_impl::RotationQuaternion<PrimType_, RotationUsage::PASSIVE>(b).toStoredImplementation()));
  }
};

//template<typename PrimType_, enum RotationUsage Usage_>
//class MultiplicationTraits<RotationBase<eigen_impl::EulerAnglesZyx<PrimType_, Usage_>, Usage_>, RotationBase<eigen_impl::EulerAnglesZyx<PrimType_, Usage_>, Usage_>> {
// public:
//  inline static eigen_impl::EulerAnglesZyx<PrimType_, Usage_> mult(const eigen_impl::EulerAnglesZyx<PrimType_, Usage_>& a, const eigen_impl::EulerAnglesZyx<PrimType_, Usage_>& b) {
//    return eigen_impl::EulerAnglesZyx<PrimType_, Usage_>(eigen_impl::RotationQuaternion<PrimType_, Usage_>(
//                                                                 eigen_impl::RotationQuaternion<PrimType_, Usage_>(a).toStoredImplementation()*
//                                                                 eigen_impl::RotationQuaternion<PrimType_, Usage_>(b).toStoredImplementation()));
//  }
//};



/* -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Rotation Traits
 * ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */
template<typename PrimType_, enum RotationUsage Usage_>
class RotationTraits<eigen_impl::EulerAnglesZyx<PrimType_, Usage_>> {
 public:
  template<typename get_matrix3X<eigen_impl::EulerAnglesZyx<PrimType_, Usage_>>::IndexType Cols>
  inline static typename get_matrix3X<eigen_impl::EulerAnglesZyx<PrimType_, Usage_>>::template Matrix3X<Cols> rotate(const eigen_impl::EulerAnglesZyx<PrimType_, Usage_>& zyx, const typename get_matrix3X<eigen_impl::EulerAnglesZyx<PrimType_, Usage_>>::template Matrix3X<Cols>& m){
    return eigen_impl::RotationMatrix<PrimType_, Usage_>(zyx).toStoredImplementation() * m;
  }
};

/* -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Comparison Traits
 * ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- */


} // namespace internal
} // namespace rotations
} // namespace kindr


#endif /* KINDR_ROTATIONS_EIGEN_EULERANGLESZYX_HPP_ */