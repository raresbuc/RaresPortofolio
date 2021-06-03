import os
import sys
import pandas as pd
import numpy as np
from matplotlib import pyplot as plt


def load_points_from_file(filename):
    """Loads 2d points from a csv called filename
    Args:
        filename : Path to .csv file
    Returns:
        (xs, ys) where xs and ys are a numpy array of the co-ordinates.
    """
    points = pd.read_csv(filename, header=None)
    return points[0].values, points[1].values


def view_data_segments(xs, ys):
    """Visualises the input file with each segment plotted in a different colour.
    Args:
        xs : List/array-like of x co-ordinates.
        ys : List/array-like of y co-ordinates.
    Returns:
        None
    """
    assert len(xs) == len(ys)
    assert len(xs) % 20 == 0
    len_data = len(xs)
    num_segments = len_data // 20
    colour = np.concatenate([[i] * 20 for i in range(num_segments)])
    plt.set_cmap('Dark2')
    plt.scatter(xs, ys, c=colour)
    plt.show()


def fit_Wh(X, Y):
    return np.linalg.inv(X.T.dot(X)).dot(X.T).dot(Y)


def cheb(xs, c):
    coefs = c * [0] + [1]
    return np.polynomial.chebyshev.chebval(xs, coefs)


def chebX(X, order):
    xs = cheb(X, 0)
    for c in range(order - 1):
        xs = np.vstack([xs, cheb(X, c + 1)])
    return xs.T


def square_error(Y, Yh_test):
    return np.sum((Y - Yh_test) ** 2)


arg_list = sys.argv
xs, ys = load_points_from_file(str(arg_list[1]))
total_error = 0
segm_nr = len(xs) // 20
fig, ax = plt.subplots()
ax.set_xlabel("$X_\lambda$")
ax.set_ylabel("$Y_\lambda$")
ax.scatter(xs, ys)


for i in range(segm_nr):
    new_X = np.array([])
    new_Y = np.array([])
    for j in range(20):
        new_X = np.append(new_X, xs[20 * i + j])
        new_Y = np.append(new_Y, ys[20 * i + j])

    error = np.array([])
    X_train = new_X[:15]
    X_test = new_X[15:]
    Y_train = new_Y[:15]
    Y_test = new_Y[15:]

    for order in range(2, 20):
        Wh = fit_Wh(chebX(X_train, order), Y_train)
        Yh_test = chebX(X_test, order).dot(Wh)
        cross_validation_error = ((Y_test - Yh_test) ** 2).mean()
        error = np.append(error, cross_validation_error)

    Yh_test = chebX(np.sin(X_test), 2).dot(fit_Wh(chebX(np.sin(X_train), 2), Y_train))
    cross_validation_error = ((Y_test - Yh_test) ** 2).mean()

    error_1 = error[0]
    i = 1
    while(error[i] < error_1):
        error_1 = error[i]
        i += 1

    if(error_1 < cross_validation_error):
        Wh = fit_Wh(chebX(X_train, i+1), Y_train)
        ax.plot(new_X, chebX(new_X, i+1).dot(Wh), 'red', label="fitted")
        total_error += square_error(new_Y, chebX(new_X, i+1).dot(Wh))
    else:
        Wh = fit_Wh(chebX(np.sin(X_train), 2), Y_train)
        ax.plot(new_X, chebX(np.sin(new_X), 2).dot(Wh), 'black', label="fitted")
        total_error += square_error(new_Y, chebX(np.sin(new_X), 2).dot(Wh))

print(total_error)

if(len(arg_list) == 3):
    if(str(arg_list[2]) == "--plot"):
        plt.show()