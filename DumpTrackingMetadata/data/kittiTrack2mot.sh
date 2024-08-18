#!/bin/bash
#######################################################################################################
# SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: LicenseRef-NvidiaProprietary
#
# NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
# property and proprietary rights in and to this material, related
# documentation and any modifications thereto. Any use, reproduction,
# disclosure or distribution of this material and related documentation
# without an express license agreement from NVIDIA CORPORATION or
# its affiliates is strictly prohibited.
#######################################################################################################

IN_DIR=
OUT_FILE=
PREFIX=
#USE_CAR=

while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -i|--input)
    IN_DIR="$2"
    shift # past argument
    shift # past value
    ;;
    -o|--outfile)
    OUT_FILE="$2"
    shift # past argument
    shift # past value
    ;;
    -p|--prefix)
    PREFIX="$2"
    shift # past argument
    shift # past value
    ;;
    -a|--allclasses)
    USE_CAR=1
    shift
    shift
    ;;
esac
done

declare -A IDMAP
if [[ $USE_CAR == 1 ]]
then
IDMAP=(
    [Person]=0
    [Car]=1
    [Bicycle]=0
    [Roadsign]=0
    [Bag]=0
    [person]=0
    [car]=1
    [bicycle]=0
    [roadsign]=0
    [bag]=0
)
else
IDMAP=(
    [Person]=1
    [Car]=0
    [Bicycle]=0
    [Roadsign]=0
    [Bag]=0
    [person]=1
    [car]=0
    [bicycle]=0
    [roadsign]=0
    [bag]=0
)
fi

usage() {
    echo "Usage: $0 -i <input kitti labels dir> -o <output file name> [-p <prefix>] [-a (convert all classes)]"
    echo ""
    echo "Ex: $0 -i ~/deepstream/labels -o ~/output/mot-labels.txt -p 00_000"
    echo "This converts all labels in ~/deepstream/labels/00_000*.txt into MOT labels to be stored in ~/output/mot-labels.txt"
}

: " KITTI Tracking Data Format

#Values    Name      Description
----------------------------------------------------------------------------
   1    frame        Frame within the sequence where the object appearers
   1    track id     Unique tracking id of this object within this sequence
   1    type         Describes the type of object: 'Car', 'Van', 'Truck',
                     'Pedestrian', 'Person_sitting', 'Cyclist', 'Tram',
                     'Misc' or 'DontCare'
   1    truncated    Integer (0,1,2) indicating the level of truncation.
                     Note that this is in contrast to the object detection
                     benchmark where truncation is a float in [0,1].
   1    occluded     Integer (0,1,2,3) indicating occlusion state:
                     0 = fully visible, 1 = partly occluded
                     2 = largely occluded, 3 = unknown
   1    alpha        Observation angle of object, ranging [-pi..pi]
   4    bbox         2D bounding box of object in the image (0-based index):
                     contains left, top, right, bottom pixel coordinates
   3    dimensions   3D object dimensions: height, width, length (in meters)
   3    location     3D object location x,y,z in camera coordinates (in meters)
   1    rotation_y   Rotation ry around Y-axis in camera coordinates [-pi..pi]
   1    score        Only for results: Float, indicating confidence in
                     detection, needed for p/r curves, higher is better.
"

[[ -z "$IN_DIR" ]] && usage && exit -1
[[ ! -d "$IN_DIR" ]] && echo "$IN_DIR is not a directory" && exit -1
[[ -z "$OUT_FILE" ]] && usage && exit -1

> $OUT_FILE
i=1
for IN_FILE in `ls -v $IN_DIR/$PREFIX*.txt`
do
    [[ "$IN_FILE" == "$OUT_FILE" ]] && continue
    while IFS=" " read -r f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 f12 f13 f14 f15 f16 f17 f18 misc
    do
	    class=$f2
        class_id=${IDMAP[$class]}
	    #test $class_id -eq 0 && continue
        if [[ $class_id == 0 ]]; then continue; fi
    	frame=$i
	    id=$f3
    	x1=${f7%.*}
    	y1=${f8%.*}
    	x2=${f9%.*}
    	y2=${f10%.*}
        width=$((x2-x1))
        height=$((y2-y1))
        confidence=$f18
        printf "%u,%d,%d,%d,%d,%d,%f,-1,-1,-1\n" \
	       "$frame" "$id" "$x1" "$y1" "$width" "$height" "$confidence" >> $OUT_FILE
    done < $IN_FILE
    ((i++))
done

((i--))
echo "Done converting $i frames"
