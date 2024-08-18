#include "pairinteraction/pairinteraction.hpp"
#include "pairinteraction/utils/args.hpp"

#include <filesystem>
#include <spdlog/spdlog.h>
#include <vector>

int main(int argc, char **argv) {
    // Call the setup function to configure logging
    pairinteraction::setup();

    // Create a database instance
    std::filesystem::path databasedir;
    bool download_missing = false;

    for (int i = 1; i < argc; ++i) {
        bool found = pairinteraction::args::parse_download_missing(i, argc, argv, download_missing);
        if (!found) {
            pairinteraction::args::parse_database(i, argc, argv, databasedir);
        }
    }

    thread_local pairinteraction::Database database(download_missing, true, databasedir);

    // Create a basis
    auto basis = pairinteraction::BasisAtomCreator<double>()
                     .set_species("Rb")
                     .restrict_quantum_number_n(58, 62)
                     .restrict_quantum_number_l(0, 2)
                     .restrict_quantum_number_m(0.5, 0.5)
                     .create(database);

    SPDLOG_INFO("Number of basis states: {}", basis->get_number_of_states());

    // Create systems for different values of the electric field
    std::vector<pairinteraction::SystemAtom<double>> systems;
    systems.reserve(10);
    for (int i = 0; i < 10; ++i) {
        auto system = pairinteraction::SystemAtom<double>(basis);
        system.set_electric_field({0, 0, i * 1e-9});
        systems.push_back(std::move(system));
    }

    // Diagonalize the systems in parallel
    pairinteraction::DiagonalizerEigen<double> diagonalizer;
    pairinteraction::diagonalize(systems, diagonalizer);

    return 0;
}
