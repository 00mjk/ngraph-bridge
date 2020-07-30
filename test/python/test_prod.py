# ==============================================================================
#  Copyright 2018-2020 Intel Corporation
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ==============================================================================
"""nGraph TensorFlow bridge prod operations test

"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import pytest
import numpy as np
import tensorflow as tf
tf.compat.v1.disable_eager_execution()

from common import NgraphTest


class TestProductOperations(NgraphTest):

    @pytest.mark.parametrize(("v1", "axis", "expected"), (
        ([3.0, 2.0], -1, [6.0]),
        ([4.0, 3.0], 0, [12.0]),
        ([[4.0, 3.0], [2.0, 2.0]], 0, [8.0, 6.0]),
        ([[4.0, 3.0], [2.0, 2.0]], -2, [8.0, 6.0]),
        ([[2.0, 3.0], [4.0, 5.0]], (0, 1), [120]),
        ([[2.0, 3.0], [4.0, 5.0]], (), [[2.0, 3.0], [4.0, 5.0]]),
    ))
    def test_prod(self, v1, axis, expected):
        tensor = tf.compat.v1.placeholder(tf.float32, shape=(None))
        assert np.allclose(np.prod(v1, axis), expected)
        out = tf.reduce_prod(tensor, axis=axis)
        sess_fn = lambda sess: sess.run([out], feed_dict={tensor: v1})
        assert np.allclose(
            self.with_ngraph(sess_fn), self.without_ngraph(sess_fn))

    @pytest.mark.parametrize(("v1", "expected"), (((2.0, 2.0), [4.0]),))
    def test_prod_no_axis(self, v1, expected):
        tensor = tf.compat.v1.placeholder(tf.float32, shape=(None))
        out = tf.reduce_prod(tensor)
        sess_fn = lambda sess: sess.run((out,), feed_dict={tensor: v1})
        assert np.allclose(self.with_ngraph(sess_fn), expected)

    @pytest.mark.parametrize(("v1", "axis", "expected"),
                             (((2.0, 2.0), 0, [4.0]),))
    def test_dynamic_axis_fallback(self, v1, axis, expected):
        tensor = tf.compat.v1.placeholder(tf.float32, shape=(None))
        tf_axis = tf.compat.v1.placeholder(tf.int32, shape=(None))
        out = tf.reduce_prod(tensor, tf_axis)
        sess_fn = lambda sess: sess.run((out,),
                                        feed_dict={
                                            tensor: v1,
                                            tf_axis: axis
                                        })
        assert np.allclose(self.with_ngraph(sess_fn), expected)

    @pytest.mark.parametrize(("v1", "axis", "expected"),
                             (([[2.0, 2.0]], 1, [[4.0]]),))
    def test_keep_dims_fallback(self, v1, axis, expected):
        tensor = tf.compat.v1.placeholder(tf.float32, shape=(None))
        out = tf.reduce_prod(tensor, axis, keepdims=True)
        sess_fn = lambda sess: sess.run((out,), feed_dict={tensor: v1})
        result = self.with_ngraph(sess_fn)
        assert np.allclose(
            len(np.array(result[0].shape)), len(np.array(v1).shape))
        assert np.allclose(result, expected)
