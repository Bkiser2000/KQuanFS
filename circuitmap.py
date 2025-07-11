# Map problem to quantum circuits and operators: 
# Import packages to build circuits

from qiskit import QuantumCircuit
from qiskit.circuit.library import PauliTwoDesign
from qiskit.quantum_info import SparsePauliOp
import numpy as np


num_qubits=30
qc = PauliTwoDesign(num_qubits=num_qubits,reps=4, seed=5, insert_barriers=True)
parameters = qc.parameters


obs = SparsePauliOp.from_sparse_list([("Z", [num_qubits-2], 1)], num_qubits=num_qubits)


# Specify circuit parameter values
np.random.seed(0) # Specify the seed for debugging purpose such that the circuit is the same very time we run it
phi_max = 0.5 * np.pi
parameter_values = np.random.uniform(-1 * phi_max, phi_max, len(parameters))
