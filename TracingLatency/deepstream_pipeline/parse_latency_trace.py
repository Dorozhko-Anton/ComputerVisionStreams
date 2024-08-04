import numpy as np
from collections import defaultdict
import matplotlib.pylab as plt
import pandas as pd
import argparse


parser = argparse.ArgumentParser(
                    prog='LatencyTraceParse',
                    description='Parses gstreamer latency trace logs',
                    epilog='')
parser.add_argument('filename')
args = parser.parse_args()

input = args.filename
measurements_by_element = defaultdict(dict)
with open(input, 'r') as f:
    data = f.readlines()

for l in data:
    if 'element-latency' in l:    
        l = l.split()
        if len(l) == 12:
            try:
                name = l[8][16:-1]
                latency = int(l[10][14:-1]) / 10**6
                ts = int(l[11][12:-1]) / 10**6
        
                if name not in measurements_by_element:
                    measurements_by_element[name]['latency'] = []
                    measurements_by_element[name]['ts'] = []
                measurements_by_element[name]['latency'].append(latency)
                measurements_by_element[name]['ts'].append(ts)
            except:
                print(l)
                break
    if ':0:: latency' in l: 
        # ['0:00:00.039067120', '84', '0x75657c000b70', 'TRACE', 'GST_TRACER', ':0::', 'latency,', 'src-element-id=(string)0x58757b2e3c80,', 'src-element=(string)videotestsrc0,', 'src=(string)src,', 'sink-element-id=(string)0x58757b3efe00,', 'sink-element=(string)fakesink0,', 'sink=(string)sink,', 'time=(guint64)645477,', 'ts=(guint64)39058461;']
        l = l.split()
        try:
            measurement_start_element = l[8][len('src-element=(string)'):-1] 
            measurement_end_element   = l[11][len('sink-element=(string)'):-1]
            latency = int(l[13][len('time=(guint64)'):-1]) / 10**6
            ts = int(l[14][len('ts=(guint64)'):-1]) / 10**6

            name = f"src({measurement_start_element})->sink({measurement_end_element})"
            if name not in measurements_by_element:
                measurements_by_element[name]['latency'] = []
                measurements_by_element[name]['ts'] = []
            measurements_by_element[name]['latency'].append(latency)
            measurements_by_element[name]['ts'].append(ts)
        except Exception as e:
            print(e)
            print(l)
            break


columns = ['element', 'median (ms)', 'avg (ms)', 'min (ms)', 'max (ms)', 'std (ms)', 'p99 (ms)']
data = []

for k, v in measurements_by_element.items():
    l = np.array(measurements_by_element[k]['latency'])

    data.append([
        k,
        np.median(l),
        np.mean(l),
        np.min(l),
        np.max(l),
        np.std(l),
        np.percentile(l, 99)
    ])

df = pd.DataFrame(data=data, columns=columns)
df = df.sort_values(by='median (ms)', ascending=False)

pd.options.display.float_format = '{:,.2f}'.format

df.to_csv("output.csv", header=True, float_format="%.2f")
print(df)