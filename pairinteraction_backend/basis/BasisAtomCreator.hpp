#pragma once

#include <complex>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "utils/traits.hpp"

template <typename Scalar>
class BasisAtom;

template <typename Real>
class KetAtom;

class Database;

/**
 * @class BasisAtomCreator
 *
 * @brief Builder class for creating BasisAtom objects.
 *
 * @tparam Scalar Complex number type.
 */
template <typename Scalar>
class BasisAtomCreator {
public:
    using real_t = typename traits::NumTraits<Scalar>::real_t;
    using ket_t = KetAtom<real_t>;
    BasisAtomCreator() = default;
    BasisAtomCreator<Scalar> &set_species(std::string value);
    BasisAtomCreator<Scalar> &restrict_energy(real_t min, real_t max);
    BasisAtomCreator<Scalar> &restrict_quantum_number_f(real_t min, real_t max);
    BasisAtomCreator<Scalar> &restrict_quantum_number_m(real_t min, real_t max);
    BasisAtomCreator<Scalar> &restrict_parity(int parity);
    BasisAtomCreator<Scalar> &restrict_quantum_number_n(int min, int max);
    BasisAtomCreator<Scalar> &restrict_quantum_number_nu(real_t min, real_t max);
    BasisAtomCreator<Scalar> &restrict_quantum_number_l(real_t min, real_t max);
    BasisAtomCreator<Scalar> &restrict_quantum_number_s(real_t min, real_t max);
    BasisAtomCreator<Scalar> &restrict_quantum_number_j(real_t min, real_t max);
    BasisAtomCreator<Scalar> &add_ket(std::shared_ptr<const ket_t> ket);
    std::shared_ptr<const BasisAtom<Scalar>> create(Database &database) const;

private:
    std::optional<std::string> species;
    std::optional<real_t> min_energy;
    std::optional<real_t> max_energy;
    std::optional<real_t> min_quantum_number_f;
    std::optional<real_t> max_quantum_number_f;
    std::optional<real_t> min_quantum_number_m;
    std::optional<real_t> max_quantum_number_m;
    std::optional<int> parity;
    std::optional<int> min_quantum_number_n;
    std::optional<int> max_quantum_number_n;
    std::optional<real_t> min_quantum_number_nu;
    std::optional<real_t> max_quantum_number_nu;
    std::optional<real_t> min_quantum_number_l;
    std::optional<real_t> max_quantum_number_l;
    std::optional<real_t> min_quantum_number_s;
    std::optional<real_t> max_quantum_number_s;
    std::optional<real_t> min_quantum_number_j;
    std::optional<real_t> max_quantum_number_j;
    std::vector<size_t> additional_ket_ids;
    std::optional<std::string> additional_ket_species;
};

extern template class BasisAtomCreator<float>;
extern template class BasisAtomCreator<double>;
extern template class BasisAtomCreator<std::complex<float>>;
extern template class BasisAtomCreator<std::complex<double>>;
