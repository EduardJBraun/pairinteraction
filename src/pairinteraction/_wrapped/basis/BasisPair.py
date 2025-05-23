from collections.abc import Collection, Iterable, Sequence
from typing import TYPE_CHECKING, Any, ClassVar, Optional, TypeVar, Union, overload

import numpy as np

from pairinteraction import _backend
from pairinteraction._wrapped.basis.Basis import BasisBase
from pairinteraction._wrapped.cpp_types import OperatorType, Parity, get_cpp_operator_type, get_cpp_parity
from pairinteraction._wrapped.ket.KetPair import (
    KetPair,
    KetPairComplex,
    KetPairReal,
)
from pairinteraction.units import QuantityAbstract, QuantityArray, QuantityScalar, QuantitySparse

if TYPE_CHECKING:
    from numpy.typing import NDArray
    from pint.facets.plain import PlainQuantity
    from scipy.sparse import csr_matrix

    from pairinteraction._wrapped.basis.BasisAtom import BasisAtom
    from pairinteraction._wrapped.ket.KetAtom import KetAtom  # noqa: F401  # required fo sphinx for KetPairLike
    from pairinteraction._wrapped.ket.KetPair import KetPairLike
    from pairinteraction._wrapped.system.SystemAtom import SystemAtom

KetPairType = TypeVar("KetPairType", bound=KetPair)
BasisPairLike = Union["BasisPair", tuple["BasisAtom", "BasisAtom"], Sequence["BasisAtom"]]

UnionCPPBasisPair = Union[_backend.BasisPairReal, _backend.BasisPairComplex]
UnionTypeCPPBasisPairCreator = Union[type[_backend.BasisPairCreatorReal], type[_backend.BasisPairCreatorComplex]]


class BasisPair(BasisBase[KetPairType]):
    """Basis for a pair of atoms.

    Add all product states of the eigenstates of two given SystemAtom objects to the basis,
    which pair energy is within the given energy range.
    You can also specify which total magnetic quantum number m the pair should have (if it is conserved)
    and the product of the parities of the two atoms.
    Due to the possible restrictions of the basis states, the BasisPair coefficients matrix will in general
    not be square but (n x d),
    where n is the number of all involved kets (typically basis1.number_of_kets * basis2.number_of_kets)
    and d is the number of basis states (after applying the restrictions).

    Examples:
        >>> import pairinteraction.real as pi
        >>> ket = pi.KetAtom("Rb", n=60, l=0, m=0.5)
        >>> basis = pi.BasisAtom("Rb", n=(58, 63), l=(0, 3))
        >>> system = pi.SystemAtom(basis).set_magnetic_field([0, 0, 1], unit="G").diagonalize()
        >>> pair_energy = 2 * system.get_corresponding_energy(ket, unit="GHz")
        >>> pair_basis = pi.BasisPair(
        ...     [system, system],
        ...     energy=(pair_energy - 3, pair_energy + 3),
        ...     energy_unit="GHz",
        ... )
        >>> print(pair_basis)
        BasisPair(|Rb:59,S_1/2,-1/2; Rb:61,S_1/2,-1/2⟩ ... |Rb:58,F_7/2,7/2; Rb:59,S_1/2,1/2⟩)

    """

    _cpp: UnionCPPBasisPair
    _cpp_creator: ClassVar[UnionTypeCPPBasisPairCreator]

    def __init__(
        self,
        systems: Collection["SystemAtom"],
        m: Optional[tuple[float, float]] = None,
        product_of_parities: Optional[Parity] = None,
        energy: Union[tuple[float, float], tuple["PlainQuantity[float]", "PlainQuantity[float]"], None] = None,
        energy_unit: Optional[str] = None,
    ) -> None:
        """Create a basis for a pair of atoms.

        Args:
            systems: tuple of two SystemAtom objects, which define the two atoms, from which the BasisPair is build.
                Both systems have to be diagonalized before creating the BasisPair.
            m: tuple of (min, max) values for the total magnetic quantum number m of the pair state.
                Default None, i.e. no restriction.
            product_of_parities: The product parity of the states to consider.
                Default None, i.e. add all available states.
            energy: tuple of (min, max) value for the pair energy. Default None, i.e. add all available states.
            energy_unit: In which unit the energy values are given, e.g. "GHz".
                Default None, i.e. energy is provided as pint object.

        """
        creator = self._cpp_creator()
        for system in systems:
            creator.add(system._cpp)  # type: ignore [reportPrivateUsage, arg-type]
        if m is not None:
            creator.restrict_quantum_number_m(*m)
        if product_of_parities is not None:
            creator.restrict_product_of_parities(get_cpp_parity(product_of_parities))
        if energy is not None:
            min_energy_au = QuantityScalar.from_pint_or_unit(energy[0], energy_unit, "ENERGY").to_base_unit()
            max_energy_au = QuantityScalar.from_pint_or_unit(energy[1], energy_unit, "ENERGY").to_base_unit()
            creator.restrict_energy(min_energy_au, max_energy_au)
        self._cpp = creator.create()

    @overload
    def get_amplitudes(self, ket_or_basis: "KetPairLike") -> "NDArray[Any]": ...

    @overload
    def get_amplitudes(self, ket_or_basis: BasisPairLike) -> "csr_matrix": ...

    def get_amplitudes(self, ket_or_basis: Union["KetPairLike", BasisPairLike]):
        ket_or_basis_cpp: list[Any]
        if not isinstance(ket_or_basis, Iterable):
            ket_or_basis_cpp = [ket_or_basis._cpp]
        else:
            ket_or_basis_cpp = [obj._cpp for obj in ket_or_basis]
        return self._cpp.get_amplitudes(*ket_or_basis_cpp)

    @overload
    def get_overlaps(self, ket_or_basis: "KetPairLike") -> "NDArray[Any]": ...

    @overload
    def get_overlaps(self, ket_or_basis: BasisPairLike) -> "csr_matrix": ...

    def get_overlaps(self, ket_or_basis: Union["KetPairLike", BasisPairLike]):
        ket_or_basis_cpp: list[Any]
        if not isinstance(ket_or_basis, Iterable):
            ket_or_basis_cpp = [ket_or_basis._cpp]
        else:
            ket_or_basis_cpp = [obj._cpp for obj in ket_or_basis]
        return self._cpp.get_overlaps(*ket_or_basis_cpp)

    @overload
    def get_matrix_elements(
        self, ket_or_basis: "KetPairLike", operators: tuple[OperatorType, OperatorType], qs: tuple[int, int]
    ) -> "PlainQuantity[NDArray[Any]]": ...

    @overload
    def get_matrix_elements(
        self, ket_or_basis: "KetPairLike", operators: tuple[OperatorType, OperatorType], qs: tuple[int, int], unit: str
    ) -> "NDArray[Any]": ...

    @overload
    def get_matrix_elements(
        self, ket_or_basis: BasisPairLike, operators: tuple[OperatorType, OperatorType], qs: tuple[int, int]
    ) -> "PlainQuantity[csr_matrix]": ...

    @overload
    def get_matrix_elements(
        self,
        ket_or_basis: BasisPairLike,
        operators: tuple[OperatorType, OperatorType],
        qs: tuple[int, int],
        unit: str,
    ) -> "csr_matrix": ...

    def get_matrix_elements(
        self,
        ket_or_basis: Union["KetPairLike", BasisPairLike],
        operators: tuple[OperatorType, OperatorType],
        qs: tuple[int, int],
        unit: Optional[str] = None,
    ):
        operators_cpp = [get_cpp_operator_type(operator) for operator in operators]
        ket_or_basis_cpp: list[Any]
        if not isinstance(ket_or_basis, Iterable):
            ket_or_basis_cpp = [ket_or_basis._cpp]
        else:
            ket_or_basis_cpp = [obj._cpp for obj in ket_or_basis]
        matrix_elements_au = self._cpp.get_matrix_elements(*ket_or_basis_cpp, *operators_cpp, *qs)
        matrix_elements: QuantityAbstract
        if isinstance(matrix_elements_au, np.ndarray):
            matrix_elements = QuantityArray.from_base_unit(matrix_elements_au, operators)
        else:  # csr_matrix
            matrix_elements = QuantitySparse.from_base_unit(matrix_elements_au, operators)
        return matrix_elements.to_pint_or_unit(unit)


class BasisPairReal(BasisPair[KetPairReal]):
    _cpp: _backend.BasisPairReal  # type: ignore [reportIncompatibleVariableOverride]
    _cpp_creator = _backend.BasisPairCreatorReal
    _TypeKet = KetPairReal


class BasisPairComplex(BasisPair[KetPairComplex]):
    _cpp: _backend.BasisPairComplex  # type: ignore [reportIncompatibleVariableOverride]
    _cpp_creator = _backend.BasisPairCreatorComplex
    _TypeKet = KetPairComplex
