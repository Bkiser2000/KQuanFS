# Execute on target hardware:

from qiskit_ibm_runtime import EstimatorV2, EstimatorOptions
from qiskit_ibm_runtime.options import DynamicalDecouplingOptions


dd_options = DynamicalDecouplingOptions(enable=True, sequence_type="XpXm")
options = EstimatorOptions(dynamical_decoupling=dd_options)

# For comparison, we first run circuit with non-optimized parameters
options.resilience_level = 0
options.default_precision = 1e-2

estimator = EstimatorV2(backend=backend, options=options)
unoptimized_job = estimator.run([(t_qc, t_obs, parameter_values)])
print(unoptimized_job.job_id())

# Next we'll use resilience level 1 to add in more error mitigation
options = EstimatorOptions(dynamical_decoupling=dd_options)

options.resilience_level = 1
options.default_precision = 1e-2

estimator = EstimatorV2(backend=backend, options=options)
optimized_job = estimator.run([(t_qc, t_obs, parameter_values)])
print(optimized_job.job_id())

# Finally we implement all available error mitigation techniques 
# with resilience level 2
options = EstimatorOptions(dynamical_decoupling=dd_options)


options.resilience_level = 2
options.default_precision = 1e-2

estimator = EstimatorV2(backend=backend, options=options)
fully_optimized_job = estimator.run([(t_qc, t_obs, parameter_values)])
print(fully_optimized_job.job_id())
