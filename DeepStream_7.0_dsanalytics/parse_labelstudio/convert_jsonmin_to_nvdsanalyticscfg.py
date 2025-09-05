import json

# Input JSON
data = json.load(open("zones.json"))

# Output lines
lines = []
lines.append("[property]")
lines.append("enable=1")
lines.append("config-width=1280")
lines.append("config-height=720")
lines.append("osd-mode=2")
lines.append("display-font-size=12")
lines.append("")
lines.append("[roi-filtering-stream-0]")
lines.append("enable=1")

for idx, zone in enumerate(data[0]["zone"]):
    name = data[0]["zone_type"][idx]  # or use f"Zone{idx}"
    pts = []
    for px, py in zone["points"]:
        # Convert % coords to pixels
        x = round(px / 100 * zone["original_width"])
        y = round(py / 100 * zone["original_height"])
        pts.extend([x, y])
    roi_str = ";".join(str(v) for v in pts)
    lines.append(f"roi-{name}={roi_str}")
    
# Optional: inverse-roi, class-id
lines.append("inverse-roi=0")
lines.append("class-id=-1")

# Write to file
with open("analytics_config.txt", "w") as f:
    f.write("\n".join(lines))

print("Saved to analytics_config.txt")
