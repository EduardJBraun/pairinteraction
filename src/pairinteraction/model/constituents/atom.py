"""Pydantic models for atoms."""

from abc import ABC
from typing import Generic, Optional, TypeVar, Union

from pydantic import Field, ValidationInfo, field_validator

from pairinteraction.model.constituents.base import BaseModelConstituent
from pairinteraction.model.states.atom import BaseModelStateAtom, ModelStateAtomMQDT, ModelStateAtomSQDT
from pairinteraction.model.types.parameter import (
    ParameterConstant,
    ParameterList,
    ParameterRange,
)
from pairinteraction.model.types.simple_types import HalfInt, Positive, PositiveZero, SpeciesString

SpinType = TypeVar("SpinType", int, HalfInt)
FType = TypeVar("FType", int, HalfInt)

UnionParameterFloat = Union[ParameterConstant[float], ParameterList[float], ParameterRange[float]]


class BaseModelAtom(BaseModelConstituent, ABC):
    """Base model representing a generic atom."""

    species: SpeciesString

    min_energy: Optional[float] = None
    max_energy: Optional[float] = None
    delta_energy: Optional[PositiveZero[float]] = None

    min_energy_after_diagonalization: Optional[float] = None
    max_energy_after_diagonalization: Optional[float] = None
    delta_energy_after_diagonalization: Optional[float] = None

    efield_x: UnionParameterFloat = Field(0, validate_default=True)
    efield_y: UnionParameterFloat = Field(0, validate_default=True)
    efield_z: UnionParameterFloat = Field(0, validate_default=True)

    bfield_x: UnionParameterFloat = Field(0, validate_default=True)
    bfield_y: UnionParameterFloat = Field(0, validate_default=True)
    bfield_z: UnionParameterFloat = Field(0, validate_default=True)

    # TODO abstract class verify that states_of_interest is implemented
    states_of_interest: list[BaseModelStateAtom] = []
    additionally_included_states: list[BaseModelStateAtom] = []

    @property
    def is_real(self) -> bool:
        """Return True if the atom can be described with a real Hamiltonian (i.e. no fields in y direction)."""
        return (self.efield_y.get_min() == self.efield_y.get_max() == 0) and (
            self.bfield_y.get_min() == self.bfield_y.get_max() == 0
        )

    @field_validator("states_of_interest", "additionally_included_states", mode="before")
    @classmethod
    def set_species_for_all_states(cls, states: list, info: ValidationInfo) -> list:
        """If states are provided, set the species of the states to the species of the atom.

        This allows to define the species only once and not for each state separately.
        """
        species = info.data.get("species")
        for state in states:
            if not isinstance(state, dict):
                continue
            if not state.setdefault("species", species) == species:
                raise ValueError("species of states must be the same as for the constituent")
        return states


class ModelAtomSQDT(BaseModelAtom, Generic[SpinType]):
    """Model representing a SQDT atom with either a integer or half integer spin (i.e. SpinType is int or HalfInt)."""

    min_n: Optional[Positive[int]] = None
    max_n: Optional[Positive[int]] = None
    delta_n: Optional[PositiveZero[int]] = None

    min_nu: Optional[Positive[float]] = None
    max_nu: Optional[Positive[float]] = None
    delta_nu: Optional[PositiveZero[float]] = None

    min_l: Optional[PositiveZero[int]] = None
    max_l: Optional[PositiveZero[int]] = None
    delta_l: Optional[PositiveZero[int]] = None

    min_s: Optional[PositiveZero[SpinType]] = None
    max_s: Optional[PositiveZero[SpinType]] = None
    delta_s: Optional[PositiveZero[int]] = None

    min_j: Optional[PositiveZero[SpinType]] = None
    max_j: Optional[PositiveZero[SpinType]] = None
    delta_j: Optional[PositiveZero[int]] = None

    min_m: Optional[SpinType] = None
    max_m: Optional[SpinType] = None
    delta_m: Optional[PositiveZero[int]] = None

    states_of_interest: list[ModelStateAtomSQDT[SpinType]] = []
    additionally_included_states: list[ModelStateAtomSQDT[SpinType]] = []


class ModelAtomMQDT(BaseModelAtom, Generic[FType]):
    """Model representing a MQDT atom with either a integer or half integer total momentum f.

    This means FType is int or HalfInt.
    """

    min_n: Optional[Positive[float]] = None
    max_n: Optional[Positive[float]] = None
    delta_n: Optional[PositiveZero[float]] = None

    min_nu: Optional[Positive[float]] = None
    max_nu: Optional[Positive[float]] = None
    delta_nu: Optional[PositiveZero[float]] = None

    min_l: Optional[PositiveZero[float]] = None
    max_l: Optional[PositiveZero[float]] = None
    delta_l: Optional[PositiveZero[float]] = None

    min_s: Optional[PositiveZero[float]] = None
    max_s: Optional[PositiveZero[float]] = None
    delta_s: Optional[PositiveZero[float]] = None

    min_j: Optional[PositiveZero[float]] = None
    max_j: Optional[PositiveZero[float]] = None
    delta_j: Optional[PositiveZero[float]] = None

    min_f: Optional[PositiveZero[FType]] = None
    max_f: Optional[PositiveZero[FType]] = None
    delta_f: Optional[PositiveZero[int]] = None

    min_m: Optional[FType] = None
    max_m: Optional[FType] = None
    delta_m: Optional[PositiveZero[int]] = None

    states_of_interest: list[ModelStateAtomMQDT[FType]] = []
    additionally_included_states: list[ModelStateAtomMQDT[FType]] = []
