# Feature Maps Selection

> This document aims to provide the reasoning behing feature maps selection.


We analysed image 2 (T1GD and T2Flair) to choose best filters to run in the whole dataset.

The following details filter file organization:

```txt
data/feature_maps/50k_3b_3sph -> t1gd feature maps
9   -> n filters
0   0.5 -> filter and weight
7   0.5
16  0.5
23  -1
30  0.5
31  0.5
36  -1
37  -1
38  0.5
data/feature_maps/50k_3b_11sph -> t2flair feature maps
6   -> n filters
0   -1.0 -> filter and weight
17  0.5
25  0.5
29  -1.0
34  0.5
39  0.5
```

## Best Filters

### 11 Adj FLAIR (Best for Edemas)

-0 -29
25 + 17 + 34 + 39

### 3 Adj T1Gd (Best for NCD and ET)

-36 -37 -23
16 + 0 + 30 + 31 +38 +7

# Stats

test 0 1 2 3

watershed + grad (alpha 0):
[Dice for label 1]: 0.955284
[Dice for label 2]: 0.618468
[Dice for label 3]: 0.423669
[Average Dice]: 0.665807

watershed + grad (alpha 0.7):
[Dice for label 1]: 0.949631
[Dice for label 2]: 0.615266
[Dice for label 3]: 0.406754
[Average Dice]: 0.657217

watershed + grad (alpha 0.5):
[Dice for label 1]: 0.946979
[Dice for label 2]: 0.610178
[Dice for label 3]: 0.421353
[Average Dice]: 0.659504

watershed + grad (alpha 0.3):
[Dice for label 1]: 0.918830
[Dice for label 2]: 0.587255
[Dice for label 3]: 0.410762
[Average Dice]: 0.638949

watershed + grad (alpha 0):
[Dice for label 1]: 0.347507
[Dice for label 2]: 0.320554
[Dice for label 3]: 0.122776
[Average Dice]: 0.263612