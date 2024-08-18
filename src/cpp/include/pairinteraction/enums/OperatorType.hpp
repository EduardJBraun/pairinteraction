#pragma once

namespace pairinteraction {
enum class OperatorType {
    ZERO,                // Empty operator
    ENERGY,              // Unperturbed Hamiltonian
    ELECTRIC_DIPOLE,     // Dipole operator
    ELECTRIC_QUADRUPOLE, // Quadrupole operator
    ELECTRIC_OCTUPOLE,   // Octupole operator
    MAGNETIC_DIPOLE,     // Magnetic dipole operator
    DIAMAGNETIC,         // Part of the diamagnetic operator
    ARBITRARY            // Arbitrary operator
};
} // namespace pairinteraction
