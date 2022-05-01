import pytest
import viren2d
import math
import pickle


def vector_test_helper(vec, zero):
    assert vec.ndim >= 2
    assert vec.ndim == zero.ndim

    # Test indexing
    with pytest.raises(IndexError):
        vec[vec.ndim]

    with pytest.raises(IndexError):
        vec[-vec.ndim - 1]

    for i in range(vec.ndim):
        assert vec[vec.ndim - i - 1] == vec[-(i+1)]

    # Make a copy for later
    cp = vec.copy()
    assert cp == vec

    # Basic arithmetics
    vec2 = vec + vec
    assert vec != vec2
    assert 2 * vec == vec2

    vec *= 2
    assert vec == vec2

    vec /= 2
    assert vec == vec2 / 2

    vec2 = vec
    assert vec == vec2
    assert vec2 == cp

    vec3 = vec + vec2 + cp;
    assert vec3 == 3 * vec

    # Distance/Length & dot product:
    dot1 = vec.dot(vec)
    dot3 = vec.dot(vec3)
    assert 3 * dot1 == pytest.approx(dot3)

    length = vec.length()
    assert math.sqrt(dot1) == pytest.approx(length)
    assert dot1 == pytest.approx(vec.length_squared())

    dist = vec.distance(zero)
    assert dist == pytest.approx(length)

    vec2 = 4.0 * vec
    dist = vec.distance(vec2)
    assert 3 * length == pytest.approx(dist)

    # Cross product (only for 3d)
    other = zero.copy()
    for i in range(vec.ndim):
      other[i] = i

    if vec.ndim != 3:
      with pytest.raises(AttributeError):
        vec.cross(other)
    else:
      cross = vec.cross(other)
      expected = (
            vec.y * other.z - vec.z * other.y,
            vec.z * other.x - vec.x * other.z,
            vec.x * other.y - vec.y * other.x)
      assert cross[0] == pytest.approx(expected[0])
      assert cross[1] == pytest.approx(expected[1])
      assert cross[2] == pytest.approx(expected[2])


def check_vector_zero(vec):
    for i in range(vec.ndim):
        assert vec[i] == pytest.approx(0)


def test_vectors():
    zero2d = viren2d.Vec2d()
    assert zero2d.ndim == 2
    check_vector_zero(zero2d)

    zero3d = viren2d.Vec3d()
    assert zero3d.ndim == 3
    check_vector_zero(zero3d)

    vector_test_helper(viren2d.Vec2d(99, -45), zero2d)

    vector_test_helper(viren2d.Vec3d(1, 2, 3), zero3d)
    vector_test_helper(viren2d.Vec3d(0.01, 0.99, -15.3), zero3d)
    vector_test_helper(viren2d.Vec3d(-171, 2, -3), zero3d)


def test_pickling():
    vecs = [viren2d.Vec2d(99, -45),
            viren2d.Vec2d(0.0001, 0.0789),
            viren2d.Vec2d(-0.03, -0.01234),
            viren2d.Vec3d(1, 2, 3),
            viren2d.Vec3d(-0.001, -0.0002, -0.0003)]

    for vec in vecs:
        data = pickle.dumps(vec)
        restored = pickle.loads(data)
        assert vec == restored
