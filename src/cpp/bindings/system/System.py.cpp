#include "./System.py.hpp"

#include "pairinteraction/system/System.hpp"
#include "pairinteraction/system/SystemAtom.hpp"
#include "pairinteraction/system/SystemClassicalLight.hpp"
#include "pairinteraction/basis/BasisAtom.hpp"

#include <nanobind/eigen/sparse.h>
#include <nanobind/nanobind.h>
#include <nanobind/stl/complex.h>
#include <nanobind/stl/shared_ptr.h>

namespace nb = nanobind;
using namespace pairinteraction;

template <typename T>
static void declare_system(nb::module_ &m, std::string const &type_name) {
    std::string pylass_name = "System" + type_name;
    using basis_t = typename System<T>::basis_t;
    using scalar_t = typename System<T>::scalar_t;
    nb::class_<System<T>, TransformationBuilderInterface<scalar_t>> pyclass(m, pylass_name.c_str());
    pyclass.def("get_basis", &System<T>::get_basis)
        .def("get_matrix", &System<T>::get_matrix)
        .def("get_transformation", &System<T>::get_transformation)
        .def("get_rotator", &System<T>::get_rotator)
        .def("get_sorter", &System<T>::get_sorter)
        .def("get_blocks", &System<T>::get_blocks)
        .def("transform",
             nb::overload_cast<const Transformation<scalar_t> &>(&System<T>::transformed,
                                                                 nb::const_))
        .def("transform", nb::overload_cast<const Sorting &>(&System<T>::transformed, nb::const_));
}

template <typename T>
static void declare_system_atom(nb::module_ &m, std::string const &type_name) {
    std::string pylass_name = "SystemAtom" + type_name;
    using basis_t = typename SystemAtom<T>::basis_t;
    nb::class_<SystemAtom<T>, System<SystemAtom<T>>> pyclass(m, pylass_name.c_str());
    pyclass.def(nb::init<std::shared_ptr<const basis_t>>());
}

void bind_system(nb::module_ &m) {
    declare_system<SystemAtom<float>>(m, "SystemAtomFloat");
    declare_system<SystemAtom<double>>(m, "SystemAtomDouble");
    declare_system<SystemAtom<std::complex<float>>>(m, "SystemAtomComplexFloat");
    declare_system<SystemAtom<std::complex<double>>>(m, "SystemAtomComplexDouble");
    declare_system_atom<float>(m, "Float");
    declare_system_atom<double>(m, "Double");
    declare_system_atom<std::complex<float>>(m, "ComplexFloat");
    declare_system_atom<std::complex<double>>(m, "ComplexDouble");
}
