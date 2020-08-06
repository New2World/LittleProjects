import tensorflow as tf

class MobileNetV2(object):
    def __init__(self,
                 inp,
                 num_classes,
                 weights_decay,
                 is_training):
        self.image_size = inp.get_shape().as_list()[1]
        pool_sz = self.image_size // 32
        self.pool_size = (pool_sz,pool_sz)
        self.norm_params = {'is_training':is_training,
                            'updates_collections':None,
                            'variables_collections':[tf.GraphKeys.TRAINABLE_VARIABLES]}
        self.regularizer = tf.contrib.layers.l2_regularizer(weights_decay)
        self.model_output = self._build_model(inp, num_classes)

    def _build_model(self, inp, num_classes):
        layer = 0
        outp = tf.contrib.layers.conv2d(inp,
                                        num_outputs=32,
                                        kernel_size=3,
                                        stride=2,
                                        activation_fn=tf.nn.relu6,
                                        normalizer_fn=tf.contrib.layers.batch_norm,
                                        normalizer_params=self.norm_params,
                                        weights_regularizer=self.regularizer,
                                        scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=16,
                                         expansion=1,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=24,
                                         expansion=6,
                                         stride=2,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=24,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=32,
                                         expansion=6,
                                         stride=2,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=32,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=32,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=64,
                                         expansion=6,
                                         stride=2,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=64,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=64,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=64,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=96,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=96,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=96,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=160,
                                         expansion=6,
                                         stride=2,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=160,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=160,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = self._inverted_bottleneck(outp,
                                         num_outputs=320,
                                         expansion=6,
                                         stride=1,
                                         weights_regularizer=self.regularizer,
                                         scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = tf.contrib.layers.conv2d(outp,
                                        num_outputs=1280,
                                        kernel_size=1,
                                        stride=1,
                                        activation_fn=tf.nn.relu6,
                                        normalizer_fn=tf.contrib.layers.batch_norm,
                                        normalizer_params=self.norm_params,
                                        weights_regularizer=self.regularizer,
                                        scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = tf.contrib.layers.avg_pool2d(outp,
                                            kernel_size=self.pool_size,
                                            scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = tf.contrib.layers.conv2d(outp,
                                        num_outputs=num_classes,
                                        kernel_size=1,
                                        stride=1,
                                        activation_fn=None,
                                        scope='MobileNetV2_{}'.format(layer))
        layer += 1
        outp = tf.contrib.layers.flatten(outp)
        return outp

    def _inverted_bottleneck(self, inp, num_outputs, expansion, stride, weights_regularizer,
                             scope):
        outp = tf.contrib.layers.conv2d(inp,
                                        num_outputs=expansion*self._get_channel(inp),
                                        kernel_size=1,
                                        stride=1,
                                        activation_fn=tf.nn.relu6,
                                        normalizer_fn=tf.contrib.layers.batch_norm,
                                        normalizer_params=self.norm_params,
                                        weights_regularizer=self.regularizer)
        outp = tf.contrib.layers.separable_conv2d(outp,
                                                  num_outputs=None,
                                                  kernel_size=3,
                                                  depth_multiplier=1,
                                                  stride=stride,
                                                  activation_fn=tf.nn.relu6,
                                                  normalizer_fn=tf.contrib.layers.batch_norm,
                                                  normalizer_params=self.norm_params,
                                                  weights_regularizer=self.regularizer)
        outp = tf.contrib.layers.conv2d(outp,
                                        num_outputs=num_outputs,
                                        kernel_size=1,
                                        stride=1,
                                        activation_fn=None,
                                        normalizer_fn=tf.contrib.layers.batch_norm,
                                        normalizer_params=self.norm_params,
                                        weights_regularizer=self.regularizer)
        if stride == 1:
            shortcut = inp
            if not self._get_channel(inp) == num_outputs:
                shortcut = tf.contrib.layers.conv2d(shortcut,
                                                    num_outputs=num_outputs,
                                                    kernel_size=1,
                                                    stride=1,
                                                    activation_fn=None)
            outp = shortcut + outp
        return outp

    def _get_channel(self, tensor):
        return tensor.get_shape().as_list()[-1]
