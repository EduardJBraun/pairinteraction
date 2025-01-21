from pairinteraction import backend
from pairinteraction.backend._backend import VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH
from pairinteraction.module_tests import run_module_tests
from pairinteraction.units import ureg

__all__ = [
    "backend",
    "ureg",
    "run_module_tests",
]

__version__ = f"{VERSION_MAJOR}.{VERSION_MINOR}.{VERSION_PATCH}"
