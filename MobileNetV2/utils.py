import os, re, glob, argparse

import numpy as np
import tensorflow as tf
from tensorflow.data import TFRecordDataset

IMAGE_SIZE = 96

def config():
    parser = argparse.ArgumentParser(description='MobileNetV2')
    parser.add_mutually_exclusive_group(required=False)

    parser.add_argument('--dataset', type=str, help='pattern of dataset files')
    parser.add_argument('--image_size', type=int, default=96)
    parser.add_argument('--nclass', type=int, default=30)
    parser.add_argument('--lr', type=float, default=.045)
    parser.add_argument('--decay_step', type=int, default=1000)
    parser.add_argument('--decay_rate', type=float, default=.98)
    parser.add_argument('--epoch', type=int, default=100)
    parser.add_argument('--batch_size', type=int, default=128)
    parser.add_argument('--modeldir', type=str, default='modeldir')
    parser.add_argument('--logdir', type=str, default='logdir')
    parser.add_argument('--train', dest='is_training', action='store_true')
    parser.add_argument('--test', dest='is_training', action='store_false')
    parser.add_argument('--no_regularization', dest='regularize', action='store_false')
    parser.add_argument('--optimizer', type=str, default='Adam')
    parser.add_argument('--retrain', dest='retrain', action='store_true')
    parser.add_argument('--gpu', type=str, default=-1, help='specify which device to use')
    parser.add_argument('--gpu_fraction', type=float, default=-1.)

    parser.set_defaults(is_training=True)
    parser.set_defaults(retrain=False)
    parser.set_defaults(regularize=True)

    args = parser.parse_args()
    if args.gpu >= 0:
        os.environ['CUDA_VISIBLE_DEVICES'] = args.gpu
    gpu_options = None
    if args.gpu_fraction >= 0:
        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=args.gpu_fraction)

    return args, gpu_options

def extract_features(serialized_example):
    feature = { "image": tf.FixedLenFeature([], tf.string),
                "keypoint": tf.FixedLenFeature([], tf.string),
    }
    parsed_example = tf.parse_single_example(serialized_example, features=feature)
    image_shape = tf.stack([IMAGE_SIZE,IMAGE_SIZE,1])
    image = tf.decode_raw(parsed_example['image'], tf.uint8, name="decode_image")
    image = tf.reshape(image, image_shape)
    image = tf.cast(image, tf.float32)
    image = tf.image.per_image_standardization(image)
    kpoint = tf.decode_raw(parsed_example['keypoint'], tf.float32, name="decode_kpoint")
    kpoint = tf.reshape(kpoint, (30,))
    return image, kpoint

def read_data(dataset_path, batch_size, image_size, repeats):
    IMAGE_SIZE = image_size
    print('[*] Reading data')
    dataset = TFRecordDataset(TFRecordDataset.list_files(dataset_path)).map(extract_features)
    dataset = dataset.batch(batch_size)
    dataset = dataset.shuffle(buffer_size=64)
    dataset = dataset.repeat(repeats)
    dataset_iterator = dataset.make_one_shot_iterator()
    image_batch, kpoint_batch = dataset_iterator.get_next()
    return image_batch, kpoint_batch

def load_checkpoint(sess, saver, checkpoint_dir):
    ckpt = tf.train.get_checkpoint_state(checkpoint_dir)
    if ckpt and ckpt.model_checkpoint_path:
        ckpt_name = os.path.basename(ckpt.model_checkpoint_path)
        saver.restore(sess, os.path.join(checkpoint_dir, ckpt_name))
        counter = int(next(re.finditer("(\d+)(?!.*\d)", ckpt_name)).group(0))
        print('[*] Success to read {}'.format(ckpt_name))
        return True, counter
    else:
        print('[*] No checkpoint found')
        return False, 0
