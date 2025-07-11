# Post-process results:

# Get unoptimized result and plot 
unoptimized_result = unoptimized_job.result()
expectation_value_unoptimized = unoptimized_result[0].data.evs
expectation_value_unoptimized_stds = unoptimized_result[0].data.stds

# Get optimized result and plot 
optimized_result = optimized_job.result()
expectation_value_optimized = optimized_result[0].data.evs
expectation_value_optimized_stds = optimized_result[0].data.stds

# Get optimized result and plot 
fully_optimized_result = fully_optimized_job.result()
expectation_value_fully_optimized = fully_optimized_result[0].data.evs
expectation_value_fully_optimized_stds = fully_optimized_result[0].data.stds

print(f"Exact Expectation Value: 0.1484375")
print(f"Unoptimized Expectation Value: {expectation_value_unoptimized} ± {expectation_value_unoptimized_stds}")
print(f"Optimized Expectation Value: {expectation_value_optimized} ± {expectation_value_optimized_stds}")
print(f"Fully Optimized Expectation Value: {expectation_value_fully_optimized} ± {expectation_value_fully_optimized_stds}")
