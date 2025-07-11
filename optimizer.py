# Optimize for target hardware:

from qiskit_ibm_runtime import QiskitRuntimeService
from qiskit.transpiler.preset_passmanagers import generate_preset_pass_manager
from qiskit.transpiler import PassManager

service = QiskitRuntimeService()
backend = service.backend('ibm_sherbrooke')



target = backend.target

# Transpile the circuit
pm = generate_preset_pass_manager(
        target=target, 
        optimization_level=3
      )

t_qc = pm.run(qc)

# Map the observables according to the transpile layout
t_obs = obs.apply_layout(t_qc.layout)
