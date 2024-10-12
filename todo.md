redo the features (tone mapping, importance sampling (pdf),  image-based lighting )

- calculate tangent and bitangent only after finding the closest hit (in GetMaterial for example?)

# Benchmark
| scene     | camera pos  | time, ms |
|-----------|-------------|----------|
| ajax      | 0.0 0.5 0.5 | 32       |
| sponza    | 0.0 3.0 1.0 | 510      |
