import pytest
import numpy as np
from random import randint

_NUM_TESTS = 10

@pytest.mark.parametrize("arr", [
    np.random.randn(*tuple(randint(1, 6) for _ in range(randint(1, 4)))).astype(np.float32) for _ in range(_NUM_TESTS)
], ids=[i for i in range(_NUM_TESTS)])
def test_relu(arr):
    import pyuTensor
    import tensorflow as tf

    pyuTensor.set_ram_total(4096)
    pyuTensor.set_meta_total(4096)
    out = pyuTensor.relu(arr)
    tf_out = tf.nn.relu(arr).numpy()
    
    assert np.allclose(out, tf_out)
