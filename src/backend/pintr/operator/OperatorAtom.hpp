#pragma once

#include "pintr/operator/Operator.hpp"
#include "pintr/utils/traits.hpp"

#include <memory>
#include <vector>

namespace pintr {
enum class OperatorType;

class Database;

template <typename Scalar>
class BasisAtom;

template <typename Real>
class KetAtom;

// Specialize CrtpTraits for OperatorAtom
template <typename T>
class OperatorAtom;

template <typename Scalar>
struct traits::CrtpTraits<OperatorAtom<Scalar>> {
    using scalar_t = Scalar;
    using real_t = typename traits::NumTraits<Scalar>::real_t;
    using ket_t = KetAtom<real_t>;
    using ketvec_t = std::vector<std::shared_ptr<const ket_t>>;
    using basis_t = BasisAtom<scalar_t>;
};

template <typename Scalar>
class OperatorAtom : public Operator<OperatorAtom<Scalar>> {
public:
    static_assert(traits::NumTraits<Scalar>::from_floating_point_v);

    using Type = OperatorAtom<Scalar>;
    using basis_t = typename traits::CrtpTraits<Type>::basis_t;

    OperatorAtom(std::shared_ptr<const basis_t> basis);
    OperatorAtom(std::shared_ptr<const basis_t> basis, OperatorType type, int q = 0);

private:
    friend class Database;

    OperatorAtom(std::shared_ptr<const basis_t> basis, OperatorType type, int q,
                 Eigen::SparseMatrix<Scalar, Eigen::RowMajor> &&matrix);

    OperatorType type;
    int q;
};

extern template class OperatorAtom<float>;
extern template class OperatorAtom<double>;
extern template class OperatorAtom<std::complex<float>>;
extern template class OperatorAtom<std::complex<double>>;
} // namespace pintr
