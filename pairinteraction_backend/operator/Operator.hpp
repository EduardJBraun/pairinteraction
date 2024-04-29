#pragma once

#include <Eigen/SparseCore>
#include <array>
#include <memory>
#include <vector>

#include "enums/SortBy.hpp"
#include "interfaces/TransformableSortable.hpp"
#include "utils/traits.hpp"

template <typename Derived>
class Operator : public TransformableSortable<typename traits::OperatorTraits<Derived>::scalar_t> {
public:
    using scalar_t = typename traits::OperatorTraits<Derived>::scalar_t;
    using real_t = typename traits::OperatorTraits<Derived>::real_t;
    using ketvec_t = typename traits::OperatorTraits<Derived>::ketvec_t;
    using basis_t = typename traits::OperatorTraits<Derived>::basis_t;

    Operator(const basis_t &basis);

    const basis_t &get_basis() const;
    const Eigen::SparseMatrix<scalar_t, Eigen::RowMajor> &get_matrix() const;
    const ketvec_t &get_kets() const;
    const Eigen::SparseMatrix<scalar_t, Eigen::RowMajor> &get_coefficients() const;

    size_t get_number_of_states() const;
    size_t get_number_of_kets() const;

protected:
    Eigen::SparseMatrix<scalar_t> impl_get_rotator(real_t alpha, real_t beta,
                                                   real_t gamma) const override;
    std::vector<int> impl_get_sorter(SortBy label) const override;
    std::vector<int> impl_get_blocks(SortBy label) const override;

    void impl_transform(const Eigen::SparseMatrix<scalar_t> &transformator) override;
    void impl_sort(const std::vector<int> &sorter) override;

    std::shared_ptr<basis_t> basis;
    Eigen::SparseMatrix<scalar_t, Eigen::RowMajor> matrix;

private:
    const Derived &derived() const;
};
