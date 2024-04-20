#include "ket/KetAtom.hpp"
#include <cmath>
#include <fmt/format.h>
#include <string>
#include <vector>

template <typename Real>
KetAtom<Real>::KetAtom(Real energy, float f, float m, int p, size_t id, std::string species, int n,
                       Real nu_exp, Real nu_std, Real l_exp, Real l_std, Real s_exp, Real s_std,
                       Real j_exp, Real j_std)
    : Ket<Real>(energy, f, m, p, id), species(species), quantum_number_n(n),
      quantum_number_nu_exp(nu_exp), quantum_number_nu_std(nu_std), quantum_number_l_exp(l_exp),
      quantum_number_l_std(l_std), quantum_number_s_exp(s_exp), quantum_number_s_std(s_std),
      quantum_number_j_exp(j_exp), quantum_number_j_std(j_std) {}

template <typename Real>
std::string KetAtom<Real>::get_label() const {
    std::string label = "";

    if (quantum_number_n > 0) {
        label += fmt::format("{:d}", quantum_number_n);
    } else {
        label += fmt::format("{:.1f}", quantum_number_nu_exp);
    }

    if (quantum_number_s_exp != 0.5) {
        label += fmt::format("^{{{:.1g}}}", 2 * quantum_number_s_exp + 1);
    }

    std::vector<std::string> quantum_number_l_labels = {"S", "P", "D", "F", "G", "H"};
    if (quantum_number_l_exp == std::rintf(quantum_number_l_exp) &&
        quantum_number_l_exp < quantum_number_l_labels.size()) {
        label += quantum_number_l_labels[static_cast<size_t>(quantum_number_l_exp)];
    } else {
        label += fmt::format("{:.1g}", quantum_number_l_exp);
    }

    if (this->quantum_number_f == std::rintf(this->quantum_number_f)) {
        label += fmt::format("_{{{}}}", this->quantum_number_f);
    } else if (2 * this->quantum_number_f == std::rintf(2 * this->quantum_number_f)) {
        label += fmt::format("_{{{}/2}}", 2 * this->quantum_number_f);
    } else {
        throw std::runtime_error("Invalid quantum number f.");
    }

    return label;
}

template <typename Real>
std::string KetAtom<Real>::get_species() const {
    return species;
}

template <typename Real>
int KetAtom<Real>::get_quantum_number_n() const {
    return quantum_number_n;
}

template <typename Real>
Real KetAtom<Real>::get_quantum_number_nu() const {
    return quantum_number_nu_exp;
}

template <typename Real>
Real KetAtom<Real>::get_quantum_number_l() const {
    return quantum_number_l_exp;
}

template <typename Real>
Real KetAtom<Real>::get_quantum_number_s() const {
    return quantum_number_s_exp;
}

template <typename Real>
Real KetAtom<Real>::get_quantum_number_j() const {
    return quantum_number_j_exp;
}

// Explicit instantiations
template class KetAtom<float>;
template class KetAtom<double>;
