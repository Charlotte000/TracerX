redo the features

# perfomance test:
## ajax | 0.0 0.5 0.5
| feature          | time  |
|------------------|-------|
| texture buffer   | 38 ms |
| + ssbo           | 33 ms |
| + ubo            | 35 ms |


## box | 0.0 0.0 10.0 | 100 samples per frame
| feature          | time  |
|------------------|-------|
| + ubo            | 62 ms |
