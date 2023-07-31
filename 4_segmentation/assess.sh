#!/bin/bash
img_id=1
./bin/main 2 data/feature_maps.txt data/input_images/image_00${img_id}_FLAIR.nii.gz \
data/seeds/image_00${img_id}_FLAIR-seeds.txt data/input_images/image_00${img_id}_T1GD.nii.gz \
data/seeds/image_00${img_id}_T1GD-seeds.txt data/output_labels/image_00${img_id}_seg.nii.gz 0.8 0 0
./bin/main 3 data/output_labels/image_00${img_id}_seg.nii.gz \
data/gt_labels/image_00${img_id}_seg.nii.gz

img_id=2
./bin/main 2 data/feature_maps.txt data/input_images/image_00${img_id}_FLAIR.nii.gz \
data/seeds/image_00${img_id}_FLAIR-seeds.txt data/input_images/image_00${img_id}_T1GD.nii.gz \
data/seeds/image_00${img_id}_T1GD-seeds.txt data/output_labels/image_00${img_id}_seg.nii.gz 0.5 1 0
./bin/main 3 data/output_labels/image_00${img_id}_seg.nii.gz \
data/gt_labels/image_00${img_id}_seg.nii.gz

img_id=3
./bin/main 2 data/feature_maps.txt data/input_images/image_00${img_id}_FLAIR.nii.gz \
data/seeds/image_00${img_id}_FLAIR-seeds.txt data/input_images/image_00${img_id}_T1GD.nii.gz \
data/seeds/image_00${img_id}_T1GD-seeds.txt data/output_labels/image_00${img_id}_seg.nii.gz 1 0 0
./bin/main 3 data/output_labels/image_00${img_id}_seg.nii.gz \
data/gt_labels/image_00${img_id}_seg.nii.gz

img_id=4
./bin/main 2 data/feature_maps.txt data/input_images/image_00${img_id}_FLAIR.nii.gz \
data/seeds/image_00${img_id}_FLAIR-seeds.txt data/input_images/image_00${img_id}_T1GD.nii.gz \
data/seeds/image_00${img_id}_T1GD-seeds.txt data/output_labels/image_00${img_id}_seg.nii.gz 1 0 0
./bin/main 3 data/output_labels/image_00${img_id}_seg.nii.gz \
data/gt_labels/image_00${img_id}_seg.nii.gz

img_id=6
./bin/main 2 data/feature_maps.txt data/input_images/image_00${img_id}_FLAIR.nii.gz \
data/seeds/image_00${img_id}_FLAIR-seeds.txt data/input_images/image_00${img_id}_T1GD.nii.gz \
data/seeds/image_00${img_id}_T1GD-seeds.txt data/output_labels/image_00${img_id}_seg.nii.gz 0.4 0 0
./bin/main 3 data/output_labels/image_00${img_id}_seg.nii.gz \
data/gt_labels/image_00${img_id}_seg.nii.gz

img_id=7
./bin/main 2 data/feature_maps.txt data/input_images/image_00${img_id}_FLAIR.nii.gz \
data/seeds/image_00${img_id}_FLAIR-seeds.txt data/input_images/image_00${img_id}_T1GD.nii.gz \
data/seeds/image_00${img_id}_T1GD-seeds.txt data/output_labels/image_00${img_id}_seg.nii.gz 0.1 0 0
./bin/main 3 data/output_labels/image_00${img_id}_seg.nii.gz \
data/gt_labels/image_00${img_id}_seg.nii.gz

img_id=8
./bin/main 2 data/feature_maps.txt data/input_images/image_00${img_id}_FLAIR.nii.gz \
data/seeds/image_00${img_id}_FLAIR-seeds.txt data/input_images/image_00${img_id}_T1GD.nii.gz \
data/seeds/image_00${img_id}_T1GD-seeds.txt data/output_labels/image_00${img_id}_seg.nii.gz 0.4 0 0
./bin/main 3 data/output_labels/image_00${img_id}_seg.nii.gz \
data/gt_labels/image_00${img_id}_seg.nii.gz

img_id=9
./bin/main 2 data/feature_maps.txt data/input_images/image_00${img_id}_FLAIR.nii.gz \
data/seeds/image_00${img_id}_FLAIR-seeds.txt data/input_images/image_00${img_id}_T1GD.nii.gz \
data/seeds/image_00${img_id}_T1GD-seeds.txt data/output_labels/image_00${img_id}_seg.nii.gz 0.7 0 0
./bin/main 3 data/output_labels/image_00${img_id}_seg.nii.gz \
data/gt_labels/image_00${img_id}_seg.nii.gz