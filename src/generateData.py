# %%
from sklearn.datasets import make_blobs
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import random
import numpy as np
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D
from scipy.stats import skewnorm

# %%
import os
if not os.path.exists('data/20m/selectivities'):
    os.makedirs('data/20m/selectivities')
if not os.path.exists('data/20m/objectSize'):
    os.makedirs('data/20m/objectSize')
if not os.path.exists('data/40m/'):
    os.makedirs('data/40m/')
if not os.path.exists('data/60m/'):
    os.makedirs('data/60m/')
if not os.path.exists('data/80m/'):
    os.makedirs('data/80m/')


# %%
random.seed(1)
def makeQueries(numOfQueries, width, height, data):
    queries = []
    for i in range(numOfQueries):
        this_index = random.randint(0, len(data))
        this_point = data[this_index]
        queries.append([this_point[0], this_point[1], this_point[0]+width, this_point[1]+height])
    return queries


def checkSelectivity(queries, data):
    query_results = [0 for _ in range(len(queries))]
    for q in range(len(queries)):
        for i in range(len(data)):
            # items = data[i].split()
            xStart = float(data[i][0])
            yStart = float(data[i][1])
            # if xStart < queries[q][0] or xStart > queries[q][2] or yStart < queries[q][1] or yStart > queries[q][3]:
                # continue
            if xStart < queries[q][2] and xStart >= queries[q][0] and yStart < queries[q][3] and yStart >= queries[q][1]:
                query_results[q]+=1
        print('Query ',q,' has ',query_results[q])

    avg_query_res = sum(query_results) / len(query_results)
    selectivity = 100 * avg_query_res / len(data)
    print(f'{selectivity:.20f}')

def pointInPolygon(x, y, query):

    if x < query[0] or x > query[2] or y < query[1] or y > query[3]:
        return False
    return True

def checkSelectivity_shapes(queries, data):
    query_results = [0 for _ in range(len(queries))]
    for q in range(len(queries)):
        for i in range(len(data)):
            # items = data[i].split()
            xStart = float(data[i][0])
            yStart = float(data[i][1])
            xEnd = float(data[i][2])
            yEnd = float(data[i][3])
            if queries[q][0] > xEnd or xStart > queries[q][2] or queries[q][1] > yEnd or yStart > queries[q][3]:
                continue
            query_results[q]+=1

        print('Query ',q,' has ',query_results[q])

    avg_query_res = sum(query_results) / len(query_results)
    selectivity = 100 * avg_query_res / len(data)
    print(selectivity)


def writeToFile(data, filename):
    out = open(filename,'w')
    for i in range(len(data)):
        for j in range(len(data[i])):
        # items = data[i].split()
            out.write(f'{data[i][j]} ')
        out.write('\n')
    out.close()

def writeDataForAir(inFile, filename):
    out = open(filename,'w')
    with open(inFile) as f:
        for line in f:
            items = line.split()
            out.write(f'{items[0]} {items[1]} {items[0]} {items[1]}\n')
    out.close()

def checkMinAndMaxValue(filename):
    minValue = 100
    maxValue = -100
    with open(filename) as f:
        for line in f:
            items = line.split()
            for i in items:
                if float(i) > maxValue:
                    maxValue = float(i)
                if float(i) < minValue:
                    minValue = float(i)
    print('Min = ',minValue,' Max = ',maxValue)


def getAverageExtent(filename, datasize):
    xExtent = 0
    yExtent = 0
    maxXExtent = -1000
    maxYExtent = -1000
    minXExtent = 1000
    minYExtent = 1000
    with open(filename) as f:
        for line in f:
            # xMin, yMin, xMax, yMax = float(line.split())
            values = [float(x) for x in line.split()]
            localXExtent = values[2] - values[0]
            localYExtent = values[3] - values[1]
            xExtent += localXExtent
            yExtent += localYExtent
            if localXExtent > maxXExtent:
                maxXExtent = localXExtent
            if localYExtent > maxYExtent:
                maxYExtent = localYExtent

            if localXExtent < minXExtent:
                minXExtent = localXExtent
            if localYExtent < minYExtent:
                minYExtent = localYExtent

    print('AVG: xExtent = ',xExtent/datasize,' yExtent = ',yExtent/datasize)
    print('MAX: xExtent = ',maxXExtent,' yExtent = ',maxYExtent)
    print('MIN: xExtent = ',minXExtent,' yExtent = ',minYExtent)


def normalizeData(queryFile, dataFile, dataNormFile, queryNormFile):
    df = pd.read_csv(queryFile,sep=' ',header=None)
    df1 = pd.read_csv(dataFile,sep=' ',header=None)

    data = df1.values
    x, y = data[:,0], data[:,1]

    queries = df.values
    xStart, yStart, xEnd, yEnd = queries[:,0], queries[:,1], queries[:,2], queries[:,3]

    dMinx = np.min(x)
    dMiny = np.min(y)
    dMaxx = np.max(x)
    dMaxy = np.max(y)

    qMinX = np.min((np.min(xStart),np.min(xEnd)))
    qMinY = np.min((np.min(yStart),np.min(yEnd)))
    qMaxX = np.max((np.max(xStart),np.max(xEnd)))
    qMaxY = np.max((np.max(yStart),np.max(yEnd)))

    minx = np.min((dMinx,qMinX))
    miny = np.min((dMiny,qMinY))
    maxx = np.max((dMaxx,qMaxX))
    maxy = np.max((dMaxy,qMaxY))

    xExtent = maxx - minx
    yExtent = maxy - miny

    xNorm = (x - minx)/xExtent
    yNorm = (y - miny)/yExtent

    qxStartNorm = (xStart - minx)/xExtent
    qyStartNorm = (yStart - miny)/yExtent
    qxEndNorm = (xEnd - minx)/xExtent
    qyEndNorm = (yEnd - miny)/yExtent

    out = open(dataNormFile,'w')
    for i in range(len(xNorm)):
        out.write(f'{xNorm[i]} {yNorm[i]}\n')
    out.close()

    out = open(queryNormFile,'w')
    for i in range(len(qxEndNorm)):
        out.write(f'{qxStartNorm[i]} {qyStartNorm[i]} {qxEndNorm[i]} {qyEndNorm[i]}\n')
    out.close()

def normalizeShapeData(queryFile, dataFile, dataNormFile, queryNormFile):
    df = pd.read_csv(queryFile,sep=' ',header=None)
    df1 = pd.read_csv(dataFile,sep=' ',header=None)

    data = df1.values
    xStart, yStart, xEnd, yEnd = data[:,0], data[:,1], data[:,2], data[:,3]

    queries = df.values
    qxStart, qyStart, qxEnd, qyEnd = queries[:,0], queries[:,1], queries[:,2], queries[:,3]

    dMinX = np.min((np.min(xStart),np.min(xEnd)))
    dMinY = np.min((np.min(yStart),np.min(yEnd)))
    dMaxX = np.max((np.max(xStart),np.max(xEnd)))
    dMaxY = np.max((np.max(yStart),np.max(yEnd)))

    qMinX = np.min((np.min(qxStart),np.min(qxEnd)))
    qMinY = np.min((np.min(qyStart),np.min(qyEnd)))
    qMaxX = np.max((np.max(qxStart),np.max(qxEnd)))
    qMaxY = np.max((np.max(qyStart),np.max(qyEnd)))

    minx = np.min((dMinX,qMinX))
    miny = np.min((dMinY,qMinY))
    maxx = np.max((dMaxX,qMaxX))
    maxy = np.max((dMaxY,qMaxY))

    xExtent = maxx - minx
    yExtent = maxy - miny

    dxStartNorm = (xStart - minx)/xExtent
    dyStartNorm = (yStart - miny)/yExtent
    dxEndNorm = (xEnd - minx)/xExtent
    dyEndNorm = (yEnd - miny)/yExtent

    qxStartNorm = (qxStart - minx)/xExtent
    qyStartNorm = (qyStart - miny)/yExtent
    qxEndNorm = (qxEnd - minx)/xExtent
    qyEndNorm = (qyEnd - miny)/yExtent

    out = open(dataNormFile,'w')
    for i in range(len(dxStartNorm)):
       out.write(f'{dxStartNorm[i]} {dyStartNorm[i]} {dxEndNorm[i]} {dyEndNorm[i]}\n')
    out.close()

    out = open(queryNormFile,'w')
    for i in range(len(qxEndNorm)):
        out.write(f'{qxStartNorm[i]} {qyStartNorm[i]} {qxEndNorm[i]} {qyEndNorm[i]}\n')
    out.close()
    


# %% [markdown]
# # Point Data

# %% [markdown]
# ## Uniform

# %%
dataSize = 20000000
dimensions = 2
points = np.random.uniform(0, 1, (dataSize, dimensions))

uniformData = list(zip(points[:, 0],points[:, 1]))

querySize = 10000
uniformQueries = makeQueries(querySize, 0.01, 0.01, uniformData)
# checkSelectivity(uniformQueries, uniformData)


# %%
writeToFile(uniformData, 'data/20m/synthetic_uniform_2D_20m.txt')
writeToFile(uniformQueries, 'data/20m/synthetic_uniform_2D_queries_10K.txt')
normalizeData('data/20m/synthetic_uniform_2D_queries_10K.txt','data/20m/synthetic_uniform_2D_20m.txt', 'data/20m/synthetic_uniform_2D_20m_norm.txt','data/20m/synthetic_uniform_2D_queries_10K_norm.txt')
writeDataForAir('data/20m/synthetic_uniform_2D_20m_norm.txt', 'data/20m/synthetic_uniform_2D_20m_norm_air.txt')

# %%
dataSize = 40000000
dimensions = 2
points = np.random.uniform(0, 1, (dataSize, dimensions))

uniformData = list(zip(points[:, 0],points[:, 1]))

querySize = 10000
uniformQueries = makeQueries(querySize, 0.01, 0.01, uniformData)
# checkSelectivity(uniformQueries, uniformData)

# %%
writeToFile(uniformData, 'data/40m/synthetic_uniform_2D_40m.txt')
writeToFile(uniformQueries, 'data/40m/synthetic_uniform_2D_queries_10K.txt')
normalizeData('data/40m/synthetic_uniform_2D_queries_10K.txt','data/40m/synthetic_uniform_2D_40m.txt', 'data/40m/synthetic_uniform_2D_40m_norm.txt','data/40m/synthetic_uniform_2D_queries_10K_norm.txt')
writeDataForAir('data/40m/synthetic_uniform_2D_40m_norm.txt', 'data/40m/synthetic_uniform_2D_40m_norm_air.txt')

# %%
dataSize = 60000000
dimensions = 2
points = np.random.uniform(0, 1, (dataSize, dimensions))

uniformData = list(zip(points[:, 0],points[:, 1]))

querySize = 10000
uniformQueries = makeQueries(querySize, 0.01, 0.01, uniformData)
# checkSelectivity(uniformQueries, uniformData)

# %%
writeToFile(uniformData, 'data/60m/synthetic_uniform_2D_60m.txt')
writeToFile(uniformQueries, 'data/60m/synthetic_uniform_2D_queries_10K.txt')
normalizeData('data/60m/synthetic_uniform_2D_queries_10K.txt','data/60m/synthetic_uniform_2D_60m.txt', 'data/60m/synthetic_uniform_2D_60m_norm.txt','data/60m/synthetic_uniform_2D_queries_10K_norm.txt')
writeDataForAir('data/60m/synthetic_uniform_2D_60m_norm.txt', 'data/60m/synthetic_uniform_2D_60m_norm_air.txt')

# %%
dataSize = 80000000
dimensions = 2
points = np.random.uniform(0, 1, (dataSize, dimensions))

uniformData = list(zip(points[:, 0],points[:, 1]))

querySize = 10000
uniformQueries = makeQueries(querySize, 0.01, 0.01, uniformData)
# checkSelectivity(uniformQueries, uniformData)

# %%
writeToFile(uniformData, 'data/80m/synthetic_uniform_2D_80m.txt')
writeToFile(uniformQueries, 'data/80m/synthetic_uniform_2D_queries_10K.txt')
normalizeData('data/80m/synthetic_uniform_2D_queries_10K.txt','data/80m/synthetic_uniform_2D_80m.txt', 'data/80m/synthetic_uniform_2D_80m_norm.txt','data/80m/synthetic_uniform_2D_queries_10K_norm.txt')
writeDataForAir('data/80m/synthetic_uniform_2D_80m_norm.txt', 'data/80m/synthetic_uniform_2D_80m_norm_air.txt')

# %% [markdown]
# ### sequential workload

# %%
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np

num_queries = 10000 
query_size = 0.05 

queries = []
for i in range(num_queries):
    x = (i + 0.5) * query_size
    y = (i + 0.5) * query_size
    if x <= 1 - query_size and y <= 1 - query_size:
        queries.append((x - query_size / 2, y - query_size / 2, query_size, query_size))

queries = np.array(queries)

with open('data/20m/synthetic_uniform_sequential_queries.txt', 'w') as f:
    for query in queries:
        f.write(f"{query[0]} {query[1]} {query[0]+query[2]} {query[3]+query[1]}\n")

plt.figure(figsize=(8, 8))
# plt.scatter(points[:, 0], points[:, 1], alpha=0.5, label='Uniform Points')

scale_factor = 1.4  # Scale the size of rectangles for visibility
for (x, y, width, height) in queries:
    rect = patches.Rectangle((x*scale_factor, y*scale_factor), width * scale_factor, height * scale_factor, edgecolor='blue', facecolor='none', linewidth=2)
    plt.gca().add_patch(rect)

plt.xlim(0, 1)
plt.ylim(0, 1)
plt.show()

# %% [markdown]
# ### zoom-in workload

# %%
# import numpy as np
# import matplotlib.pyplot as plt
# import matplotlib.patches as patches

num_queries = 1000    # Number of zooming queries
initial_query_size = 0.85  # Initial size of the query rectangle
shrink_factor = 0.98  # Factor by which the size shrinks in each step
center_point = (0.5, 0.5)  # Center point to zoom into


queries = []
size = initial_query_size

for i in range(num_queries):
    # Calculate the current rectangle's bottom-left corner
    x = center_point[0] - size / 2
    y = center_point[1] - size / 2
    queries.append((x, y, size, size))
    
    # Decrease size for the next rectangle
    size *= shrink_factor

queries = np.array(queries)

selectivity_counts = []
for (x, y, width, height) in queries:
    count = np.sum((points[:, 0] >= x) & (points[:, 0] <= x + width) & 
                   (points[:, 1] >= y) & (points[:, 1] <= y + height))
    selectivity_counts.append(count)

average_selectivity = np.mean(selectivity_counts)

with open('data/20m/synthetic_uniform_zoomin_queries.txt', 'w') as f:
    for query in queries:
        f.write(f"{query[0]} {query[1]} {query[0]+query[2]} {query[3]+query[1]}\n")

# Plot the points and queries
plt.figure(figsize=(8, 8))
plt.scatter(points[:, 0], points[:, 1], alpha=0.5, label='Uniform Points')

for (x, y, width, height) in queries:
    rect = patches.Rectangle((x, y), width, height, 
                             edgecolor='red', facecolor='none', linewidth=2)
    plt.gca().add_patch(rect)

plt.xlim(0, 1)
plt.ylim(0, 1)
plt.show()


# %% [markdown]
# ## Clustered

# %%
import numpy as np
import matplotlib.pyplot as plt
from sklearn.datasets import make_blobs

np.random.seed(42)
datasize = 20000000
n_clusters = 5

centers = np.array([[0.2, 0.2],
                    [0.2, 0.8],
                    [0.8, 0.2],
                    [0.8, 0.8],
                    [0.5, 0.5]])  

X, y = make_blobs(n_samples=datasize, centers=centers, cluster_std=0.04, random_state=42)


print(np.min(X[:,0]), np.min(X[:,1]))
print(np.max(X[:,0]), np.max(X[:,1]))

plt.scatter(X[:, 0], X[:, 1], c=y, s=30)
plt.show()



# %%
fig = plt.figure(figsize=(10, 10))
ax = fig.add_subplot(111,projection='3d')
ax.set_xlabel('x')
ax.set_ylabel('y')
x, y = X[:,0], X[:,1]
print(np.max(x),np.max(y))
number_of_bins = 100
minx = np.min(x)
miny = np.min(y)
maxx = np.max(x)
maxy = np.max(y)
hist, xedges, yedges = np.histogram2d(x, y, bins=number_of_bins, range=[[minx, maxx-0.03], [miny, maxy-0.03]])

xpos, ypos = np.meshgrid(xedges[:-1], yedges[:-1], indexing="ij")
xpos = xpos.ravel()
ypos = ypos.ravel()
zpos = 0

dx = dy = 0.03 * np.ones_like(zpos)
dz = hist.ravel()

ax.bar3d(xpos, ypos, zpos, dx, dy, dz, zsort='average')

plt.show()

# %%
clusteredData = list(zip(X[:,0].tolist(),X[:,1].tolist()))
querySize = 10000
clusteredQueries = makeQueries(querySize, 0.0034, 0.0034, clusteredData)
# checkSelectivity(clusteredQueries, clusteredData)
writeToFile(clusteredData,'data/20m/synthetic_clustered_2D_20m.txt')
writeToFile(clusteredQueries,'data/20m/synthetic_clustered_2D_queries_10K.txt')
normalizeData('data/20m/synthetic_clustered_2D_queries_10K.txt','data/20m/synthetic_clustered_2D_20m.txt', 'data/20m/synthetic_clustered_2D_20m_norm.txt', 'data/20m/synthetic_clustered_2D_queries_10K_norm.txt')
writeDataForAir('data/20m/synthetic_clustered_2D_20m_norm.txt', 'data/20m/synthetic_clustered_2D_20m_norm_air.txt')

# %% [markdown]
# ## Skewed

# %%
from scipy.stats import skewnorm

dimensions = 2
alpha = 20 # skewness parameter
datasize = 20000000

#generate data from skewnorm dist
skewedNormData = skewnorm.rvs(alpha, size=(datasize, dimensions))

# x = skewedNormData[:, 0]
# y = skewedNormData[:, 1]
# a = plt.scatter(x, y)

# %%
querySize = 10000
skewedNormQueries = makeQueries(querySize, 0.02, 0.02, skewedNormData)
# checkSelectivity(skewedNormQueries, skewedNormData)
writeToFile(skewedNormData,'data/20m/synthetic_skewNormal_2D_20m.txt')
writeToFile(skewedNormQueries,'data/20m/synthetic_skewNormal_2D_queries_10K.txt')
normalizeData('data/20m/synthetic_skewNormal_2D_queries_10K.txt','data/20m/synthetic_skewNormal_2D_20m.txt', 'data/20m/synthetic_skewNormal_2D_20m_norm.txt', 'data/20m/synthetic_skewNormal_2D_queries_10K_norm.txt')
writeDataForAir('data/20m/synthetic_skewNormal_2D_20m_norm.txt','data/20m/synthetic_skewNormal_2D_20m_norm_air.txt')

# %%
df = pd.read_csv('data/20m/synthetic_skewNormal_2D_20m_norm.txt',sep=' ',header=None)
data = df.values

# x = data[:, 0]
# y = data[:, 1]
# a = plt.scatter(x, y)

#make the necessary imports
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D


#plot 3D distribution-histogramm of data
fig = plt.figure(figsize=(10, 10))
ax = fig.add_subplot(111,projection='3d')
ax.set_xlabel('x')
ax.set_ylabel('y')
# x, y = data[:,0], data[:,1]
print(np.max(x),np.max(y))
number_of_bins = 100
minx = np.min(x)
miny = np.min(y)
maxx = np.max(x)
maxy = np.max(y)
hist, xedges, yedges = np.histogram2d(x, y, bins=number_of_bins, range=[[minx, maxx-0.03], [miny, maxy-0.03]])

xpos, ypos = np.meshgrid(xedges[:-1], yedges[:-1], indexing="ij")
xpos = xpos.ravel()
ypos = ypos.ravel()
zpos = 0

dx = dy = 0.03 * np.ones_like(zpos)
dz = hist.ravel()

ax.set_xlim([0,1])
ax.set_ylim([0,1])
ax.bar3d(xpos, ypos, zpos, dx, dy, dz, zsort='average')

plt.show()

skewedNormData = list(zip(x,y))

# %% [markdown]
# # Shape Data

# %% [markdown]
# ## Uniform

# %%
datasize = 20000000
dimensions = 2

# Generate random center points in the range [0, 1]
center_points = np.random.uniform(0, 1, (datasize, dimensions))

# Create shapes by extending the center points
shapes = []
extension_range = np.random.uniform(0, 0.002, (datasize, dimensions))  # Extensions in [0, 0.002]

uniformShapeData = []
for center, extension in zip(center_points, extension_range):
    shape = np.array([
        center - extension,  # Corner 1 (min)
        center + extension  # Corner 2 (max)
    ])
    uniformShapeData.append([shape[0][0], shape[0][1], shape[1][0], shape[1][1]])


# fig, ax = plt.subplots()
# for i in range(len(uniformShapeData[:10000])):
# 	# plt.gca().add_patch(Rectangle((xlow[i],ylow[i]),xhigh[i]-xlow[i],yhigh[i]-ylow[i],fill=True, alpha=0.3, zorder=100, figure=fig))
# 	plt.gca().add_patch(Rectangle((uniformShapeData[i][0],uniformShapeData[i][1]),(uniformShapeData[i][2]-uniformShapeData[i][0])*10,(uniformShapeData[i][3]-uniformShapeData[i][1])*10,fill=True, alpha=0.3, zorder=100, figure=fig))
# 	# label = ax.annotate(i, xy=((uniformShapeData[i][0]+uniformShapeData[i][2])/2, (uniformShapeData[i][1]+uniformShapeData[i][3])/2), fontsize=5, ha="center")
# ax.autoscale()
# plt.show()

# %%
querySize = 10000
uniformShapeQueries = makeQueries(querySize, 0.01, 0.01, uniformShapeData)
# checkSelectivity(uniformShapeQueries, uniformShapeData)
writeToFile(uniformShapeData, 'data/20m/synthetic_uniform_shapes_2D_20m.txt')
writeToFile(uniformShapeQueries, 'data/20m/synthetic_uniform_shapes_2D_queries_10K.txt')
normalizeShapeData('data/20m/synthetic_uniform_shapes_2D_queries_10K.txt','data/20m/synthetic_uniform_shapes_2D_20m.txt', 'data/20m/synthetic_uniform_shapes_2D_20m_norm.txt','data/20m/synthetic_uniform_shapes_2D_queries_10K_norm.txt')

# %%


# %% [markdown]
# ## Skewed

# %%
datasize = 20000000
dimensions = 2
alpha = 20

center_points = skewnorm.rvs(alpha, size=(datasize, dimensions))

shapes = []
extension_range = np.random.uniform(0, 0.002, (datasize, dimensions))  # Extensions in [0, 0.002]

skewedShapeData = []
for center, extension in zip(center_points, extension_range):
    shape = np.array([
        center - extension,   # Corner 2 (min)
        center + extension  # Corner 1 (max)
    ])
    skewedShapeData.append([shape[0][0], shape[0][1], shape[1][0], shape[1][1]])

    
# fig, ax = plt.subplots()
# for i in range(len(skewedShapeData[:10000])):
# 	# print(i)
# 	# plt.gca().add_patch(Rectangle((xlow[i],ylow[i]),xhigh[i]-xlow[i],yhigh[i]-ylow[i],fill=True, alpha=0.3, zorder=100, figure=fig))
# 	plt.gca().add_patch(Rectangle((skewedShapeData[i][0],skewedShapeData[i][1]),(skewedShapeData[i][2]-skewedShapeData[i][0])*50,(skewedShapeData[i][3]-skewedShapeData[i][1])*50,fill=True, alpha=0.3, zorder=100, figure=fig))
# 	# label = ax.annotate(i, xy=((clusteredShapeData[i][0]+clusteredShapeData[i][2])/2, (clusteredShapeData[i][1]+clusteredShapeData[i][3])/2), fontsize=5, ha="center")
# ax.autoscale()
# plt.show()

# %% [markdown]
# # Clustered

# %%
np.random.seed(42)
datasize = 20000000
n_clusters = 5

centers = np.array([[0.2, 0.2],
                    [0.2, 0.8],
                    [0.8, 0.2],
                    [0.8, 0.8],
                    [0.5, 0.5]])  

X, y = make_blobs(n_samples=datasize, centers=centers, cluster_std=0.04, random_state=42)

width = 0.002
height = 0.002
data = list(zip(X[:,0].tolist(),X[:,1].tolist()))
clusteredShapeData = []
for i in range(len(data)):
    rndWidth = random.uniform(0,width)
    rndHeight = random.uniform(0,height)
    clusteredShapeData.append([data[i][0]-rndWidth, data[i][1]-rndHeight, data[i][0]+rndWidth, data[i][1]+rndHeight])


# fig, ax = plt.subplots()
# for i in range(len(clusteredShapeData)):
#   # print(i)
#   # plt.gca().add_patch(Rectangle((xlow[i],ylow[i]),xhigh[i]-xlow[i],yhigh[i]-ylow[i],fill=True, alpha=0.3, zorder=100, figure=fig))
#   plt.gca().add_patch(Rectangle((clusteredShapeData[i][0],clusteredShapeData[i][1]),(clusteredShapeData[i][2]-clusteredShapeData[i][0])*30,(clusteredShapeData[i][3]-clusteredShapeData[i][1])*30,fill=True, alpha=0.3, zorder=100, figure=fig))
#   # label = ax.annotate(i, xy=((clusteredShapeData[i][0]+clusteredShapeData[i][2])/2, (clusteredShapeData[i][1]+clusteredShapeData[i][3])/2), fontsize=5, ha="center")
# ax.autoscale()
# plt.show()

# %%
querySize = 10000
clusteredShapeQueries = makeQueries(querySize, 0.0001, 0.00001, clusteredShapeData)
# checkSelectivity_shapes(clusteredShapeQueries, clusteredShapeData)
writeToFile(clusteredShapeData, 'data/20m/synthetic_clustered_shapes_2D_20m.txt')
writeToFile(clusteredShapeQueries, 'data/20m/synthetic_clustered_shapes_2D_queries_10K.txt')
normalizeShapeData('data/20m/synthetic_clustered_shapes_2D_queries_10K.txt','data/20m/synthetic_clustered_shapes_2D_20m.txt', 'data/20m/synthetic_clustered_shapes_2D_20m_norm.txt','data/20m/synthetic_clustered_shapes_2D_queries_10K_norm.txt')

# %%


# %%


# %% [markdown]
# # Workloads with different selectivity

# %%
# Selectivity 0.01%
querySize = 10000
uniformQueries = makeQueries(querySize, 0.0105, 0.0105, uniformData)
# checkSelectivity(uniformQueries, uniformData)

# %%
writeToFile(uniformQueries, 'data/20m/selectivities/synthetic_uniform_2D_queries_selectivity001_10K.txt')
normalizeData('data/20m/selectivities/synthetic_uniform_2D_queries_selectivity001_10K.txt','data/20m/synthetic_uniform_2D_20m.txt', 'data/20m/synthetic_uniform_2D_20m_norm.txt','data/20m/selectivities/synthetic_uniform_2D_queries_selectivity001_10K_norm.txt')
#writeDataForAir('data/20m/selectivities/synthetic_uniform_2D_20m_norm.txt', 'data/20m/selectivities/synthetic_uniform_2D_20m_norm_air.txt')

# %%
# Selectivity 0.001%
querySize = 10000
uniformQueries = makeQueries(querySize, 0.004, 0.004, uniformData)
# checkSelectivity(uniformQueries, uniformData)

# %%
writeToFile(uniformQueries, 'data/20m/selectivities/synthetic_uniform_2D_queries_selectivity0001_10K.txt')
normalizeData('data/20m/selectivities/synthetic_uniform_2D_queries_selectivity0001_10K.txt','data/20m/synthetic_uniform_2D_20m.txt', 'data/20m/synthetic_uniform_2D_20m_norm.txt','data/20m/selectivities/synthetic_uniform_2D_queries_selectivity0001_10K_norm.txt')

# %%
# Selectivity 0.1%
querySize = 10000
uniformQueries = makeQueries(querySize, 0.011, 0.011, uniformData)
# checkSelectivity(uniformQueries, uniformData)

# %%
writeToFile(uniformQueries, 'data/20m/selectivities/synthetic_uniform_2D_queries_selectivity01_10K.txt')
normalizeData('data/20m/selectivities/synthetic_uniform_2D_queries_selectivity01_10K.txt','data/20m/synthetic_uniform_2D_20m.txt', 'data/20m/synthetic_uniform_2D_20m_norm.txt','data/20m/selectivities/synthetic_uniform_2D_queries_selectivity01_10K_norm.txt')

# %%
# Selectivity 0.0001%
querySize = 10000
uniformQueries = makeQueries(querySize, 0.00105, 0.00105, uniformData)
# checkSelectivity(uniformQueries, uniformData)

# %%
writeToFile(uniformQueries, 'data/20m/selectivities/synthetic_uniform_2D_queries_selectivity00001_10K.txt')
normalizeData('data/20m/selectivities/synthetic_uniform_2D_queries_selectivity00001_10K.txt','data/20m/synthetic_uniform_2D_20m.txt', 'data/20m/synthetic_uniform_2D_20m_norm.txt','data/20m/selectivities/synthetic_uniform_2D_queries_selectivity00001_10K_norm.txt')

# %%
# Selectivity 0.0001%
querySize = 10000
uniformQueries = makeQueries(querySize, 0.1, 0.1, uniformData)
# checkSelectivity(uniformQueries, uniformData)

# %% [markdown]
# # Shapes of different size

# %% [markdown]
# ## 0.005

# %%
datasize = 20000000
dimensions = 2

# Generate random center points in the range [0, 1]
center_points = np.random.uniform(0, 1, (datasize, dimensions))

# Create shapes by extending the center points
shapes = []
extension_range = np.random.uniform(0, 0.01, (datasize, dimensions))  # Extensions in [0, 0.002]

uniformShapeData = []
for center, extension in zip(center_points, extension_range):
    # Create corners of the hyperrectangle
    shape = np.array([
        center - extension,  # Corner 1 (min)
        center + extension  # Corner 2 (max)
    ])
    uniformShapeData.append([shape[0][0], shape[0][1], shape[1][0], shape[1][1]])

# %%
# Selectivity 0.01%
# For 0.01 and 0.05 I used 0.001 for query extents
# For 0.005 I used 0.01
querySize = 10000
uniformShapeQueries = makeQueries(querySize, 0.001, 0.001, uniformShapeData)
# checkSelectivity_shapes(uniformShapeQueries, uniformShapeData)

# %%
writeToFile(uniformShapeData, 'data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size001.txt')
writeToFile(uniformShapeQueries, 'data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size001_10K.txt')
normalizeShapeData('data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size001_10K.txt','data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size001.txt', 'data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size001_norm.txt','data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size001_10K_norm.txt')

# %% [markdown]
# ## 0.05

# %%
datasize = 20000000
dimensions = 2

# Generate random center points in the range [0, 1]
center_points = np.random.uniform(0, 1, (datasize, dimensions))

# Create shapes by extending the center points
shapes = []
extension_range = np.random.uniform(0, 0.05, (datasize, dimensions))  # Extensions in [0, 0.002]

uniformShapeData = []
for center, extension in zip(center_points, extension_range):
    # Create corners of the hyperrectangle
    shape = np.array([
        center - extension,  # Corner 1 (min)
        center + extension  # Corner 2 (max)
    ])
    uniformShapeData.append([shape[0][0], shape[0][1], shape[1][0], shape[1][1]])

# %%
# Selectivity 0.01%
# For 0.01 and 0.05 I used 0.001 for query extents
# For 0.005 I used 0.01
querySize = 10000
uniformShapeQueries = makeQueries(querySize, 0.001, 0.001, uniformShapeData)
# checkSelectivity_shapes(uniformShapeQueries, uniformShapeData)

# %%
writeToFile(uniformShapeData, 'data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size005.txt')
writeToFile(uniformShapeQueries, 'data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size005_10K.txt')
normalizeShapeData('data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size005_10K.txt','data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size005.txt', 'data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size005_norm.txt','data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size005_10K_norm.txt')

# %% [markdown]
# ## 0.005

# %%
datasize = 20000000
dimensions = 2

# Generate random center points in the range [0, 1]
center_points = np.random.uniform(0, 1, (datasize, dimensions))

# Create shapes by extending the center points
shapes = []
extension_range = np.random.uniform(0, 0.005, (datasize, dimensions))  # Extensions in [0, 0.002]

uniformShapeData = []
for center, extension in zip(center_points, extension_range):
    # Create corners of the hyperrectangle
    shape = np.array([
        center - extension,  # Corner 1 (min)
        center + extension  # Corner 2 (max)
    ])
    uniformShapeData.append([shape[0][0], shape[0][1], shape[1][0], shape[1][1]])

# %%
writeToFile(uniformShapeData, 'data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size0005.txt')
writeToFile(uniformShapeQueries, 'data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size0005_10K.txt')
normalizeShapeData('data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size0005_10K.txt','data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size0005.txt', 'data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size0005_norm.txt','data/20m/objectSize/synthetic_uniform_shapes_2D_queries_size0005_10K_norm.txt')

# %%
getAverageExtent('data/20m/objectSize/synthetic_uniform_shapes_2D_20m_size0005_norm.txt', 20000000)



