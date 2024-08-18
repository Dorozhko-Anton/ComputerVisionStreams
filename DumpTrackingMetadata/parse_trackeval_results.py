import pandas as pd
import re

def set_column_as_index_by_id(df, column_id):
    return df.set_index(df.columns[column_id])

with open("trackeval.txt", 'r') as f:
    lines = f.readlines()

dfs = {}
for i in range(len(lines)):
    if lines[i].startswith('HOTA:'):
        columns = re.split(r"\s{2,}", lines[i].strip())
        data = []
        while True:
            i+=1
            row = re.split(r"\s{2,}", lines[i].strip())
            data.append(row)    
            if lines[i].startswith('COMBINED'):
                break      
        df = pd.DataFrame(data, columns=columns)
        df = set_column_as_index_by_id(df, 0)
        dfs['HOTA'] = df
        
    if lines[i].startswith('CLEAR:'):
        columns = re.split(r"\s{2,}", lines[i].strip())
        data = []
        while True:
            i+=1
            row = re.split(r"\s{2,}", lines[i].strip())
            data.append(row)    
            if lines[i].startswith('COMBINED'):
                break
        df = pd.DataFrame(data, columns=columns)
        df = set_column_as_index_by_id(df, 0)
        dfs['CLEAR'] = df
                
    if lines[i].startswith("Identity:"):
        columns = re.split(r"\s{2,}", lines[i].strip())
        data = []
        while True:
            i+=1
            row = re.split(r"\s{2,}", lines[i].strip())
            data.append(row)    
            if lines[i].startswith('COMBINED'):
                break
        df = pd.DataFrame(data, columns=columns)
        df = set_column_as_index_by_id(df, 0)
        dfs['IDF'] = df
        

for k, v in dfs.items():
    v.to_csv(f"{k}.csv")
