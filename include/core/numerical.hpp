////////////////////////////////////////////////////////////////////
//                                                                //
//    Copyright (c) 2019-20, UK Atomic Energy Authority (UKAEA)   //
//                                                                //
////////////////////////////////////////////////////////////////////

/*!
    @file
    Defines the core numerical operations for all data manipulation within of the library.

    @copyright UK Atomic Energy Authority (UKAEA) - 2019-20
*/
#ifndef CORE_NUMERICAL_HPP
#define CORE_NUMERICAL_HPP

#include <vector>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "common.hpp"
#include "numericalfunctions.hpp"
#include "numericalmacros.h"

PEAKINGDUCK_NAMESPACE_START(peakingduck)
PEAKINGDUCK_NAMESPACE_START(core)

    const int ArrayTypeDynamic = Eigen::Dynamic;

    template<typename Scalar, int Size=ArrayTypeDynamic>
    using Array1D = Eigen::Array<Scalar, Size, 1>;

    using Array1Di = Array1D<int>;
    using Array1Df = Array1D<float>;
    using Array1Dd = Array1D<double>;
  
    /*!
       @brief Represents a 1-dimensional data structure (basically a 1D Eigen array)
        Dynamic array - most use cases will be determined at runtime (I am assuming).
        We don't want anyone to know we are using Eigen beyond this file, since (in theory)
        it should make it easier to change library if need be. We only really need the array
        datastructure from Eigen and not much else and instead of reinventing the wheel,
        we wrap Eigen array.

        We wrap this with private inheritance on the Eigen type but there
        are a lot of methods to expose, easy to add when/if we need them. 
        
        Eigen array is pretty good, it has things like sqrt, exp on array coefficients, but 
        we need to extend this to other functions, so we use CRTP to do this.

        For all of this, you may ask why not just use Eigen and use an alias?
        Well for one, we don't need all of Eigen just the array, and not all
        of the array type (we require a simpler interface). Additionally, at some
        point we may wish to use another data structure as std::array for example.
        In this case we just change the NumericalData class to wrap that instead.
        If we change the alias this could break existing interfaces and APIs, causing
        big changes later on. Since this datastructure is fundamental to everything
        we need to make sure that we have this sorted properly first!
    */
    template<typename T=double, int Size=ArrayTypeDynamic>
    class NumericalData : private Array1D<T, Size>, 
                          public NumericalFunctions<NumericalData<T, Size>>
    {
        public:
            using BaseEigenArray = Array1D<T, Size>;

            // typedef Array1Dd Base;
            using BaseEigenArray::BaseEigenArray;

            // This constructor allows you to construct Derived type from Eigen expressions
            template<typename OtherDerived>
            NumericalData(const Eigen::ArrayBase<OtherDerived>& other)
                : BaseEigenArray(other)
            { }

            // issue constructing with no arguments
            // this aims to fix it
            explicit NumericalData()
            { 
                from_vector(std::vector<T>());
            }

            // This constructor allows you to construct from a std::vector
            explicit NumericalData(const std::vector<T>& other)
            { 
                from_vector(other);
            }

            // This method allows you to assign Eigen expressions to Derived type
            template<typename OtherDerived>
            NumericalData& operator=(const Eigen::ArrayBase <OtherDerived>& other)
            {
                this->BaseEigenArray::operator=(other);
                return *this;
            }

            // This method allows you to assign Eigen expressions to Derived type
            NumericalData& operator=(const std::vector<T>& other)
            {
                from_vector(other);
                return *this;
            }

            // clang does not like this, but gcc does,
            // not sure why, but maybe not needed?
            // using BaseEigenArray::Base;

            // operations such as (x > 0).all()
            using BaseEigenArray::Base::all;
            using BaseEigenArray::Base::any;
            using BaseEigenArray::Base::count;

            using BaseEigenArray::Base::Ones;

            // essential operations on arrays
            using BaseEigenArray::operator<<;
            using BaseEigenArray::operator>;
            using BaseEigenArray::operator<;
            using BaseEigenArray::operator=;
            using BaseEigenArray::operator==;
            using BaseEigenArray::operator*;
            using BaseEigenArray::operator*=;
            using BaseEigenArray::operator+;
            using BaseEigenArray::operator+=;
            using BaseEigenArray::operator-;
            using BaseEigenArray::operator-=;
            using BaseEigenArray::operator/;
            using BaseEigenArray::operator/=;
            using BaseEigenArray::operator new;
            using BaseEigenArray::operator delete;

            PEAKINGDUCK_NUMERICAL_OPERATOR_IMP_MACRO(NumericalData,BaseEigenArray,+)
            PEAKINGDUCK_NUMERICAL_OPERATOR_IMP_MACRO(NumericalData,BaseEigenArray,-)
            PEAKINGDUCK_NUMERICAL_OPERATOR_IMP_MACRO(NumericalData,BaseEigenArray,*)
            PEAKINGDUCK_NUMERICAL_OPERATOR_IMP_MACRO(NumericalData,BaseEigenArray,/)

            // entry access operations
            using BaseEigenArray::operator[];
            using BaseEigenArray::operator();
            using BaseEigenArray::data;
            using BaseEigenArray::size;

            inline void from_vector(const std::vector<T>& raw){
                this->BaseEigenArray::operator=(BaseEigenArray::Map(raw.data(), raw.size()));
            }

            inline std::vector<T> to_vector() const{
                return std::vector<T>(this->data(), this->data() + this->size());
            }

            // some useful predefined methods 
            // map
            using BaseEigenArray::exp;
            using BaseEigenArray::log;
            using BaseEigenArray::sqrt;
            using BaseEigenArray::square;
            using BaseEigenArray::pow;
            using BaseEigenArray::replicate;
            using BaseEigenArray::reverse;
            using BaseEigenArray::reverseInPlace;

            // reduce
            using BaseEigenArray::mean;
            using BaseEigenArray::sum;
            using BaseEigenArray::maxCoeff;
            using BaseEigenArray::minCoeff;

            // custom unary operations
            using BaseEigenArray::unaryExpr;
    };

PEAKINGDUCK_NAMESPACE_END
PEAKINGDUCK_NAMESPACE_END

#endif // CORE_NUMERICAL_HPP