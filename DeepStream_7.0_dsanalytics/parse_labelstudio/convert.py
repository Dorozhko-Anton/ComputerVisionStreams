import json
import math
import argparse

def poly_to_pixels(points, width, height):
    new_points = []
    for pt in points:
        new_points.append(int(pt[0] * width / 100))
        new_points.append(int(pt[1] * height / 100))
    return new_points

def rect_to_line_and_direction(r):
    cx, cy = r["x"], r["y"]
    w, h = r["width"], r["height"]
    angle = math.radians(r.get("rotation", 0))

    if w >= h:
        length = w
        dx = math.cos(angle) * length / 2
        dy = math.sin(angle) * length / 2
    else:
        length = h
        dx = math.cos(angle + math.pi / 2) * length / 2
        dy = math.sin(angle + math.pi / 2) * length / 2

    # Line endpoints
    x0 = int(cx)
    y0 = int(cy)
    x1 = int(cx + 2 * dx)
    y1 = int(cy + 2 *dy)

    # Direction vector
    mid_x = (x0 + x1) // 2
    mid_y = (y0 + y1) // 2
    dir_x = int(mid_x + dy / 4)
    dir_y = int(mid_y - dx / 4)

    return (x0, y0, x1, y1), (mid_x, mid_y, dir_x, dir_y)

def generate_nvds_config(input_file, output_file, width=1280, height=720):
    with open(input_file) as f:
        data = json.load(f)

    objects = {}
    for ann in data["annotations"]:
        for res in ann["result"]:
            obj_id = res["id"]
            if obj_id not in objects:
                objects[obj_id] = {}
            value = res["value"]

            if "polygonlabels" in value:
                objects[obj_id]["label"] = value["polygonlabels"][0]
                objects[obj_id]["points"] = value["points"]

            if "choices" in value:
                inverse = value["choices"][0][0]
                if inverse.startswith("inverse-roi="):
                    objects[obj_id]["inverse_roi"] = int(inverse.split("=")[1])
            if "number" in value:
                objects[obj_id]["threshold"] = value["number"]

            if "text" in value:
                objects[obj_id]["name"] = value["text"][0]

            if "rectanglelabels" in value:
                objects[obj_id]["rect"] = {
                    "label": value["rectanglelabels"][0],
                    "x": value["x"] * width / 100,
                    "y": value["y"] * height / 100,
                    "width": value["width"] * width / 100,
                    "height": value["height"] * height / 100,
                    "rotation": value.get("rotation", 0)
                }

    config_lines = [
        "# The values in the config file are overridden by values set through GObject properties.",
        "[property]",
        "enable=1",
        f"config-width={width}",
        f"config-height={height}",
        "osd-mode=2",
        "display-font-size=12",
        ""
    ]

    roi_lines, oc_lines, lc_lines, dir_lines = [], [], [], []

    for obj in objects.values():
        if "points" in obj and "label" in obj:
            pts = poly_to_pixels(obj["points"], width, height)
            if obj["label"] == "ROI-Filter":
                inverse = obj.get("inverse_roi", 0)
                roi_lines.append(f"roi-{obj.get('name','ROI')}=" + ";".join(map(str, pts)))
                roi_lines.append(f"inverse-roi={inverse}")
                roi_lines.append("class-id=-1\n")
            elif obj["label"] == "Overcrowding":
                threshold = obj.get("threshold", 5)
                oc_lines.append(f"roi-{obj.get('name','OC')}=" + ";".join(map(str, pts)))
                oc_lines.append(f"object-threshold={threshold}")
                oc_lines.append("class-id=-1\n")

        if "rect" in obj:
            r = obj["rect"]
            if r["label"] == "LineCrossing":
                line_pts, dir_pts = rect_to_line_and_direction(r)
                lc_lines.append(
                    f"line-crossing-{obj.get('name','LC')}=" + ";".join(map(str, dir_pts)) + ";" + ";".join(map(str, line_pts))
                )
                lc_lines.append("class-id=0")
                lc_lines.append("extended=0")
                lc_lines.append("mode=loose\n")
            elif r["label"] == "Direction":
                line_pts, _ = rect_to_line_and_direction(r)
                dir_lines.append(f"direction-{obj.get('name','Direction')}=" + ";".join(map(str, line_pts)))
                dir_lines.append("class-id=0\n")

    if roi_lines:
        config_lines.append("[roi-filtering-stream-0]")
        config_lines.append("enable=1")
        config_lines.extend(roi_lines)
    if oc_lines:
        config_lines.append("[overcrowding-stream-0]")
        config_lines.append("enable=1")
        config_lines.extend(oc_lines)
    if lc_lines:
        config_lines.append("[line-crossing-stream-0]")
        config_lines.append("enable=1")
        config_lines.extend(lc_lines)
    if dir_lines:
        config_lines.append("[direction-detection-stream-0]")
        config_lines.append("enable=1")
        config_lines.extend(dir_lines)

    with open(output_file, "w") as f:
        f.write("\n".join(config_lines))

    print(f"NVDS config generated: {output_file}")

def main():
    parser = argparse.ArgumentParser(description="Convert LabelStudio JSON to NVDS config")
    parser.add_argument("input", help="Input LabelStudio JSON file")
    parser.add_argument("output", help="Output NVDS config file")
    parser.add_argument("--width", type=int, default=1280, help="Config width (default: 1280)")
    parser.add_argument("--height", type=int, default=720, help="Config height (default: 720)")
    args = parser.parse_args()

    generate_nvds_config(args.input, args.output, args.width, args.height)

if __name__ == "__main__":
    main()
