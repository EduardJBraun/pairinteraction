#include "pairinteraction/basis/BasisCombinedCreator.hpp"

#include "pairinteraction/basis/BasisAtom.hpp"
#include "pairinteraction/basis/BasisCombined.hpp"
#include "pairinteraction/enums/Parity.hpp"
#include "pairinteraction/enums/TransformationType.hpp"
#include "pairinteraction/ket/KetAtom.hpp"
#include "pairinteraction/ket/KetCombined.hpp"
#include "pairinteraction/system/SystemAtom.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fmt/core.h>
#include <limits>
#include <memory>

namespace pairinteraction {
template <typename Scalar>
BasisCombinedCreator<Scalar> &
BasisCombinedCreator<Scalar>::add(const SystemAtom<Scalar> &system_atom) {
    if (!system_atom.is_diagonal()) {
        throw std::invalid_argument("The system must be diagonalized before it can be added.");
    }
    systems_atom.push_back(system_atom);
    return *this;
}

template <typename Scalar>
BasisCombinedCreator<Scalar> &BasisCombinedCreator<Scalar>::restrict_energy(real_t min,
                                                                            real_t max) {
    range_energy = {min, max};
    return *this;
}

template <typename Scalar>
BasisCombinedCreator<Scalar> &BasisCombinedCreator<Scalar>::restrict_quantum_number_m(real_t min,
                                                                                      real_t max) {
    range_quantum_number_m = {min, max};
    return *this;
}

template <typename Scalar>
std::shared_ptr<const BasisCombined<Scalar>> BasisCombinedCreator<Scalar>::create() const {
    if (systems_atom.size() != 2) {
        throw std::invalid_argument(
            "Two SystemAtom must be added before creating the combined basis.");
    }

    real_t numerical_precision = 10 * std::numeric_limits<real_t>::epsilon();

    // Sort the states, which are eigenstates, by their energy
    auto system1 = systems_atom[0].get();
    auto system2 = systems_atom[1].get();
    system1.transform(system1.get_sorter({TransformationType::SORT_BY_ENERGY}));
    system2.transform(system2.get_sorter({TransformationType::SORT_BY_ENERGY}));

    // Construct the canonical basis that contains all combined states with allowed energies and
    // quantum numbers
    auto basis1 = system1.get_basis();
    auto basis2 = system2.get_basis();
    auto eigenvalues1 = system1.get_eigenvalues();
    auto eigenvalues2 = system2.get_eigenvalues();
    real_t *eigenvalues2_begin = eigenvalues2.data();
    real_t *eigenvalues2_end = eigenvalues2_begin + eigenvalues2.size();
    size_t number_of_states2 = basis2->get_number_of_states();
    bool has_quantum_number_m = basis1->has_quantum_number_m() && basis2->has_quantum_number_m();

    ketvec_t kets;
    kets.reserve(eigenvalues1.size() * eigenvalues2.size());

    typename basis_t::map_range_t map_range_of_state_index2;
    map_range_of_state_index2.reserve(eigenvalues1.size());

    // Loop only over states with an allowed energy
    for (size_t idx1 = 0; idx1 < static_cast<size_t>(eigenvalues1.size()); ++idx1) {

        // Get the energetically allowed range of the second index
        size_t min = 0;
        size_t max = eigenvalues2.size();
        if (range_energy.is_finite()) {
            real_t min_val2 = range_energy.min() - eigenvalues1[idx1];
            real_t max_val2 = range_energy.max() - eigenvalues1[idx1];
            min = std::distance(eigenvalues2_begin,
                                std::lower_bound(eigenvalues2_begin, eigenvalues2_end, min_val2));
            max = std::distance(eigenvalues2_begin,
                                std::upper_bound(eigenvalues2_begin, eigenvalues2_end, max_val2));
        }
        map_range_of_state_index2.emplace(idx1, typename basis_t::range_t(min, max));

        // Loop over the energetically allowed range of the second index
        for (size_t idx2 = min; idx2 < max; ++idx2) {
            // Get energy
            const real_t energy = eigenvalues1[idx1] + eigenvalues2[idx2];
            assert(!range_energy.is_finite() ||
                   (energy >= range_energy.min() && energy <= range_energy.max()));

            // Get quantum numbers
            Parity parity = Parity::UNKNOWN;
            real_t quantum_number_f = std::numeric_limits<real_t>::max();
            real_t quantum_number_m = std::numeric_limits<real_t>::max();
            if (has_quantum_number_m) {
                quantum_number_m =
                    basis1->get_quantum_number_m(idx1) + basis2->get_quantum_number_m(idx2);
                if (range_quantum_number_m.is_finite() &&
                    (quantum_number_m < range_quantum_number_m.min() - numerical_precision ||
                     quantum_number_m > range_quantum_number_m.max() + numerical_precision)) {
                    continue;
                }
            } else if (range_quantum_number_m.is_finite()) {
                throw std::invalid_argument(
                    "The quantum number m must not be restricted because it is not well-defined.");
            }

            // Get ket with largest overlap
            std::shared_ptr<const KetAtom<real_t>> ket1 = basis1->get_corresponding_ket(idx1);
            std::shared_ptr<const KetAtom<real_t>> ket2 = basis2->get_corresponding_ket(idx2);

            // Store the combined state as a ket. The combined, linearized index is the ID of the
            // ket.
            kets.emplace_back(std::make_shared<ket_t>(
                typename ket_t::Private(), idx1 * number_of_states2 + idx2, energy,
                quantum_number_f, quantum_number_m, parity,
                std::vector<std::shared_ptr<const Ket<real_t>>>{ket1, ket2}));
        }
    }

    kets.shrink_to_fit();

    auto now = std::chrono::high_resolution_clock::now();
    std::uint64_t nanoseconds =
        std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    std::string id_of_kets = fmt::format("{:016x}", nanoseconds);

    return std::make_shared<basis_t>(typename basis_t::Private(), std::move(kets),
                                     std::move(id_of_kets), std::move(map_range_of_state_index2),
                                     basis1, basis2);
}

// Explicit instantiations
template class BasisCombinedCreator<float>;
template class BasisCombinedCreator<double>;
template class BasisCombinedCreator<std::complex<float>>;
template class BasisCombinedCreator<std::complex<double>>;
} // namespace pairinteraction
