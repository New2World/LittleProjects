import numpy as np
import cv2

def impreprocess(img, scale=0.5):
  if not scale == 1.0:
    img = cv2.resize(img, dsize=None, fx=scale, fy=scale, interpolation=cv2.INTER_AREA)
  y = cv2.split(cv2.cvtColor(img, cv2.COLOR_BGR2YUV))[0]
  bim = cv2.adaptiveThreshold(cv2.medianBlur(y, 5), 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 83, 2)
  bim = cv2.morphologyEx(bim, cv2.MORPH_OPEN, (7,7))
  return bim

def countHierarchy(x, hiera, dp):
  x = hiera[x][2]
  cnt = 1
  cks = dp.keys()
  while x >= 0:
    if x in cks:
      return cnt + dp[x]
    cnt += 1
    x = hiera[x][2]
  return cnt

def isRectangle(contour, var=2):
  S = cv2.contourArea(contour)
  L = cv2.arcLength(contour, closed=True)
  return abs(16-1.0*L**2/S) <= var

def cascadeRectangle(x, contour, hiera):
  cnt = 0
  while x >= 0:
    if isRectangle(contour[x]):
      cnt += 1
    x = hiera[x][2]
  return cnt == 3

def getRotatedPoints(points, M):
  return points @ M[:,:2].T + M[:,2]

def calibrateAngle(points):
  points = np.asarray(points)
  upper, left = points.min(axis=0)
  lower, right = points.max(axis=0)
  mid = [(upper + lower) / 2, (left + right) / 2]
  region = np.sum((points / mid).astype(np.int32), axis=0)
  region_s = np.sum(region)
  n = 0
  if region_s == 4:
    n = 2
  elif region_s == 3:
    if region[0] == 2:
      n = 1
    else:
      n = 3
  return n * 90.0


rim = cv2.imread('qr.jpg')
max_edge = np.max(rim.shape)
imcent = tuple([x//2 for x in rim.shape[:2]])[::-1]
bim = impreprocess(rim, 1.0)

contour, hierarchy = cv2.findContours(bim, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

hiecnt = {}
locinfo = []
for i, h in enumerate(hierarchy[0]):
  if i in hiecnt.keys():
    continue
  hiecnt[i] = countHierarchy(i, hierarchy[0], hiecnt)
  if hiecnt[i] >= 3 and cascadeRectangle(i, contour, hierarchy[0]):
    locinfo.append(cv2.minAreaRect(contour[i]))

rotangle = 90 - np.abs(np.mean([t[2] for t in locinfo]))
rotmat = cv2.getRotationMatrix2D(imcent, rotangle, 1.0)
locpts = [cv2.boxPoints(t) for t in locinfo]
loccent = [getRotatedPoints(np.mean(t, axis=0), rotmat) for t in locpts]

rotmat = cv2.getRotationMatrix2D(imcent, rotangle+calibrateAngle(loccent), 1.0)

locbor = [getRotatedPoints(m, rotmat).astype(np.int32) for m in locpts]

left_bound = np.min([np.asarray(p).squeeze().min(axis=0)[0] for p in locbor])
right_bound = np.max([np.asarray(p).squeeze().max(axis=0)[0] for p in locbor])
upper_bound = np.min([np.asarray(p).squeeze().min(axis=0)[1] for p in locbor])
lower_bound = np.max([np.asarray(p).squeeze().max(axis=0)[1] for p in locbor])

locbor = [m - [left_bound, upper_bound] for m in locbor]

rotimg = cv2.warpAffine(bim, rotmat, (max_edge, max_edge))[upper_bound:lower_bound, left_bound:right_bound]

bor_img = np.stack([rotimg.copy()]*3, axis=2)
bor_img = cv2.drawContours(bor_img, locbor, -1, (0,0,255), 3)

while True:
  cv2.imshow("Image", bor_img)
  if cv2.waitKey(0) == 27:
    break

cv2.destroyAllWindows()
