import os,glob,time
import argparse

import numpy as np
import tensorflow as tf

import utils
from mobile import MobileNetV2

def train_op_fn(model,
                gtruth,
                lr,
                decay_step,
                decay_rate,
                global_step,
                regularize,
                optimizer):
    prediction = model.model_output
    mse_loss = tf.losses.mean_squared_error(labels=gtruth,predictions=prediction)
    if regularize:
        l2_loss = tf.add_n(tf.get_collection(tf.GraphKeys.REGULARIZATION_LOSSES))
        loss = mse_loss + l2_loss
    else:
        loss = mse_loss

    lr = tf.train.exponential_decay(lr,
                                    global_step=global_step,
                                    decay_steps=decay_step,
                                    decay_rate=decay_rate,
                                    staircase=True)

    if optimizer == 'Adam':
        opt = tf.train.AdamOptimizer(lr)
    elif optimizer == 'RMSProp':
        opt = tf.train.RMSPropOptimizer(lr, momentum=0.9)
    else:
        raise ValueError('Invalid optimizer')

    update_ops = tf.get_collection(tf.GraphKeys.UPDATE_OPS)
    with tf.control_dependencies(update_ops):
        train_op = opt.minimize(loss, global_step=global_step)

    tf.summary.scalar('loss', loss)
    summaries = tf.summary.merge_all()

    return train_op, loss, summaries, prediction

def train(image_batch,
          gtruth_batch,
          num_classes,
          lr,
          decay_step,
          decay_rate,
          regularize,
          optimizer,
          model_dir,
          log_dir,
          retrain,
          gpu_options):
    sess = tf.Session(config=tf.ConfigProto(gpu_options=gpu_options))
    global_step = tf.Variable(0, trainable=False)
    print('[*] Building model')
    mobilenetv2 = MobileNetV2(image_batch, num_classes, 0.00004, True)
    train_op, loss, summary, logits = train_op_fn(mobilenetv2,
                                                  gtruth_batch,
                                                  lr,
                                                  decay_step,
                                                  decay_rate,
                                                  global_step,
                                                  regularize,
                                                  optimizer)
    summary_writer = tf.summary.FileWriter(log_dir, sess.graph)
    saver = tf.train.Saver()
    sess.run(tf.global_variables_initializer())
    if not retrain:
        print('[*] Trying to load checkpoint')
        checkpoint_loaded, init_step = utils.load_checkpoint(sess, saver, model_dir)
    else:
        init_step = 0
    print('[*] Training')
    while True:
        start_time = time.time()
        try:
            _, loss_val, summary_val, step = sess.run([train_op, loss, summary, global_step])
        except tf.errors.OutOfRangeError:
            break
        end_time = time.time()
        if step % 10 == 0:
            print(  'step: {0}, loss: {1:.6f}, time-per-batch: {2:.4f}'
                    .format(step,
                            loss_val,
                            end_time-start_time))
        if step % 100 == 0:
            summary_writer.add_summary(summary_val, step)
        if step % 1000 == 0:
            saver.save(sess, os.path.join(model_dir, 'model'), global_step=global_step)
            print('Model saved at step {}'.format(step))
    saver.save(sess, os.path.join(model_dir, 'final_model'), global_step=global_step)
    sess.close()
    print('[*] Final model saved')
    print('[*] Training finish')

def evaluate(image_batch,
             gtruth_batch,
             image_size,
             num_classes,
             model_dir,
             gpu_options):
    sess = tf.Session(config=tf.ConfigProto(gpu_options=gpu_options))
    print('[*] Building model')
    mobilenetv2 = MobileNetV2(image_batch, num_classes, 0.00004, False)
    saver = tf.train.Saver()
    sess.run(tf.global_variables_initializer())
    checkpoint_loaded,_ = utils.load_checkpoint(sess, saver, model_dir)
    gtruth = None
    prediction = None
    print "[*] Evaluating"
    while True:
        try:
            gt, pred = sess.run([gtruth_batch, mobilenetv2.model_output])
        except tf.errors.OutOfRangeError:
            break
        if gtruth is None:
            gtruth = gt.copy()
        else:
            gtruth = np.vstack((gtruth, gt))
        if prediction is None:
            prediction = pred.copy()
        else:
            prediction = np.vstack((prediction, pred))
    sess.close()
    nan_list = np.isnan(gtruth)
    gtruth[nan_list] = 0.
    prediction[nan_list] = 0.
    gtruth = gtruth * image_size
    prediction = prediction * image_size
    rmse = np.sqrt(np.mean((gtruth - prediction)**2, axis=1))
    return gtruth, prediction, rmse

def main():
    args, gpu_options = utils.config()
    print args
    print

    if args.is_training:
        image_batch, gtruth_batch = utils.read_data(args.dataset,
                                                    args.batch_size,
                                                    args.image_size,
                                                    args.epoch)
        train(image_batch,
              gtruth_batch,
              args.nclass,
              args.lr,
              args.decay_step,
              args.decay_rate,
              args.regularize,
              args.optimizer,
              args.modeldir,
              args.logdir,
              args.retrain,
              gpu_options)
    else:
        image_batch, gtruth_batch = utils.read_data(args.dataset,
                                                    args.batch_size,
                                                    args.image_size, 1)
        gtruth, prediction, rmse = evaluate(image_batch,
                                            gtruth_batch,
                                            args.image_size,
                                            args.nclass,
                                            args.modeldir,
                                            gpu_options)
        print "[*] Result"
        rmse = np.delete(rmse, np.argmax(rmse))
        rmse = np.delete(rmse, np.argmin(rmse))
        print "Max RMSE: {}".format(np.max(rmse))
        print "Min RMSE: {}".format(np.min(rmse))
        print "Mean RMSE: {}".format(np.mean(rmse))

if __name__ == '__main__':
    main()
