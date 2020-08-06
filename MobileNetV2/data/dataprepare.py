import numpy as np
import pandas as pd
import cv2
import argparse
import matplotlib.pyplot as plt

import tensorflow as tf

parser = argparse.ArgumentParser(description='MobileNetV2')
parser.add_mutually_exclusive_group(required=False)

parser.add_argument('--raw_data', type=str, default="training.csv")
parser.add_argument('--image_size', type=int, default=96)
parser.add_argument('--augmentation', action="store_true")
parser.add_argument('--valid', type=float, default=0.)

parser.set_defaults(augmentation=False)

def read_from_csv(path, image_size):
    with open(path) as fd:
        kpoint_features = fd.readline().split(',')[:-1]
    dataset = pd.read_csv(path, ',', header=0)
    image_raw = dataset['Image'].tolist()
    image = np.array([int(pixel) for i in range(7049)
                                 for pixel in image_raw[i].split()])
    image = image.reshape((-1,96,96,1)).astype(np.float32)
    if image_size != 96:
        image = [cv2.resize(im, (image_size,image_size)).reshape((image_size,image_size,1)) for im in image]
        image = np.array(image)
    kpoints = np.array([dataset[feats].tolist() for feats in kpoint_features]).T
    # kpoints[np.isnan(kpoints)] = -.000096
    # kpoints = kpoints / 96
    return image, kpoints

def __bytes_feature(value):
    return tf.train.Feature(bytes_list=tf.train.BytesList(value=[value]))

def __int64_feature(value):
    return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))

def hflip_img(img):
    return img[:,::-1]

def hflip_kpt(kpt):
    kpt[np.arange(1,31,2)] = 1. - kpt[np.arange(1,31,2)]
    return kpt

def write_tfrecord(images, kpoints, image_size, tfrecord_file, augmentation, valid):
    def write_file(img, kpt, tfrecord_writer):
        img = img.astype(np.uint8).flatten().tobytes()
        kpt = kpt.astype(np.float32).tobytes()
        feature = {
            "image": __bytes_feature(img),
            "keypoint": __bytes_feature(kpt),
        }
        features = tf.train.Features(feature=feature)
        example = tf.train.Example(features=features)
        tfrecord_writer.write(example.SerializeToString())

    if images.shape[0] != kpoints.shape[0]:
        assert("number of samples is not match")
    tfrecord_writer = tf.python_io.TFRecordWriter(tfrecord_file+'-train'+'.tfrecord')
    train_sample = images.shape[0]
    if valid > 0.:
        train_sample *= 1 - valid
        train_sample = int(train_sample)
        valid_sample = images.shape[0] - train_sample
    tsample = 0
    nan_list = np.isnan(kpoints)
    for i in range(train_sample):
        pb = np.random.rand()
        if True in nan_list[i]:
            continue
        write_file(images[i], kpoints[i] / 96., tfrecord_writer)
        if augmentation and pb > .5:
            write_file(hflip_img(images[i]), hflip_kpt(kpoints[i] / 96.), tfrecord_writer)
            tsample += 2
        else:
            tsample += 1
    print "Training set: {} samples".format(tsample)
    tfrecord_writer.close()
    if valid > 0.:
        print "Validation set: {} samples".format(valid_sample)
        tfrecord_writer = tf.python_io.TFRecordWriter(tfrecord_file+'-valid'+'.tfrecord')
        for j in range(valid_sample):
            write_file(images[j + train_sample], kpoints[j + train_sample] / 96., tfrecord_writer)
        tfrecord_writer.close()

if __name__ == "__main__":
    args = parser.parse_args()
    image, kpoint = read_from_csv(args.raw_data, image_size=args.image_size)
    write_tfrecord( image,
                    kpoint,
                    args.image_size,
                    "dataset-%d" % args.image_size,
                    args.valid)
