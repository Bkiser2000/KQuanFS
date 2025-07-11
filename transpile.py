from qiskit import QuantumCircuit, QuantumRegister
from quiskit.transpiler.preset_passmanagers import generate_preset_pass_manager
from quiskit_ibm_runtime import QuiskitRuntimeService

qubits = QuantumRegister(2, name="q")
circuit = QuantumCircuit(qubits)

q0, q1 = qubits
circuit.h(q0)
circuit.cx(q0, q1)

service = QuiskitRuntimeService()
backend = service.backend("ibm_brisbane")
pass_manager = generate_preset_pass_manager(1, backend=backend)
transpiled = pass_manager.run(circuit)

transpiled.draw("mpl", idle_wires=False)
