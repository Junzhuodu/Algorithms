import matplotlib.pyplot as plt 
from mpl_toolkits.mplot3d import Axes3D
import numpy as np 

def compute_plane(xyz):
	"""
	 Computes plane coefficients a,b,c,d of the plane in the form ax+by+cz+d = 0

	 Arguments:
	 xyz -- tensor of dimension (3, N), contains points needed to fit plane.

	 Returns:
	 p -- tensor of dimension (1, 4) containing the plane parameters a,b,c,d	
	"""

	ctr = xyz.mean(axis=1)
	normalized = xyz - ctr[:, np.newaxis]
	M = np.dot(normalized, normalized.T)

	p = np.linalg.svd(M)[0][:, -1]
	d = np.matmul(p, ctr)

	p = np.append(p, -d)

	return p

def dist_to_plane(plane, x, y, z):
    """
    Computes distance between points provided by their x, and y, z coordinates
    and a plane in the form ax+by+cz+d = 0

    Arguments:
    plane -- tensor of dimension (4,1), containing the plane parameters [a,b,c,d]
    x -- tensor of dimension (Nx1), containing the x coordinates of the points
    y -- tensor of dimension (Nx1), containing the y coordinates of the points
    z -- tensor of dimension (Nx1), containing the z coordinates of the points

    Returns:
    distance -- tensor of dimension (N, 1) containing the distance between points and the plane
    """
    a, b, c, d = plane 

    return (a * x + b * y + c * z + d) / np.sqrt(a**2 + b**2 + c**2)

def ransac_plane_fit(xyz_data):
    """
    Computes plane coefficients a,b,c,d of the plane in the form ax+by+cz+d = 0
    using ransac for outlier rejection.

    Arguments:
    xyz_data -- tensor of dimension (3, N), contains all data points from which random sampling will proceed.
    num_itr -- 
    distance_threshold -- Distance threshold from plane for a point to be considered an inlier.

    Returns:
    p -- tensor of dimension (1, 4) containing the plane parameters a,b,c,d
    """

    num_itr = 100
    min_num_inliers = 10000
    distance_threshold = 0.3 

    max_inliers_cnt = 0
    max_inliers_set_idx = None 
    for i in range(num_itr):
    	# step 1: choose a minimum of 3 points from xyz_data at random
    	idx = np.random.choice(xyz_data.shape[1], 3, replace=False)
    	curr_data = xyz_data[:, idx]
    	# step 2: compute the plane model
    	plane_param = compute_plane(curr_data)
    	# step 3: find the numbers of inliers
    	distance_list = dist_to_plane(plane_param, xyz_data[0, :], xyz_data[1, :], xyz_data[2, :])
    	inliers_cnt = np.sum(distance_list < distance_threshold)
    	# step 4: check if the current number of inliers is greater than the largest in previous
    	if inliers_cnt > max_inliers_cnt:
    		max_inliers_cnt = inliers_cnt
    		max_inliers_set_idx = np.arange(xyz_data.shape[1])[distance_list < distance_threshold]
    	# step 5: check if stopping criterion if satisfied and break:
    	if inliers_cnt > min_num_inliers:
    		break

    final_data = xyz_data[:, max_inliers_set_idx]
    output_plane = compute_plane(final_data)

    return output_plane

N_POINTS = 1000
TARGET_X_SLOPE = 2
TARGET_Y_SLOPE = 3
TARGET_OFFSET = 5
EXTENTS = 2
NOISE = 2

# create random data
xs = [np.random.uniform(2*EXTENTS)-EXTENTS for i in range(N_POINTS)]
ys = [np.random.uniform(2*EXTENTS)-EXTENTS for j in range(N_POINTS)]
zs = []
for i in range(N_POINTS):
	zs.append(xs[i]*TARGET_X_SLOPE + \
			  ys[i]*TARGET_Y_SLOPE + \
			  TARGET_OFFSET + np.random.normal(scale=NOISE))

#plot raw data
plt.figure()
ax = plt.subplot(111, projection='3d')
ax.scatter(xs, ys, zs, color='b')

# compute the plane
points = np.vstack((xs, ys, zs))
fit = ransac_plane_fit(points)

errors = np.array(zs) + (np.array(xs) * fit[0] + np.array(ys) * fit[1] + fit[3]) / fit[2]
residual = np.linalg.norm(errors)
print ("solution:")
print ("%f x + %f y + %f z + %f d = 0" % (fit[0], fit[1], fit[2], fit[3]))
# print ("errors:")
# print (errors)
print ("residual:")
print (residual)

# plot plane
xlim = ax.get_xlim()
ylim = ax.get_ylim()
X,Y = np.meshgrid(np.arange(xlim[0], xlim[1]),
                  np.arange(ylim[0], ylim[1]))
Z = np.zeros(X.shape)
for r in range(X.shape[0]):
    for c in range(X.shape[1]):
        Z[r,c] = -(fit[0] * X[r,c] + fit[1] * Y[r,c] + fit[3]) / fit[2]
ax.plot_wireframe(X,Y,Z, color='k')

ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')
plt.show()

