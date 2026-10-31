# Math conventions

These conventions hold across the whole library. Code in `lib/` must follow them, and any change to them needs a decision record in [decisions/](decisions/).

## Coordinates

Points are 2D image coordinates `(x, y)` in OpenCV's convention: `x` is the column and grows to the right, `y` is the row and grows downwards, and the origin is the top-left pixel.

## Affine transforms

An affine transform is stored as a `cv::Matx23d`:

```
A = | a00 a01 tx |
    | a10 a11 ty |
```

It maps a point `p = (x, y)` to

```
A·p = | a00·x + a01·y + tx |
      | a10·x + a11·y + ty |
```

The left 2x2 block holds rotation, scale and shear. The last column is the translation. This is the same layout that `cv::estimateAffine2D` returns and `cv::warpAffine` accepts.

## Homogeneous form

`to_homogeneous(A)` appends the row `[0 0 1]`, which gives a `cv::Matx33d`:

```
H = | a00 a01 tx |
    | a10 a11 ty |
    |  0   0   1 |
```

This turns affine transforms into ordinary matrix products, and the 3x3 form is what a later switch to homographies would use.

## Composition order

`compose(second, first)` returns the transform that applies `first`, then `second`:

```
compose(second, first) = second · first      (as 3x3 homogeneous matrices)
compose(second, first)·p = second·(first·p)
```

The arguments are written in the same order as the matrix product, not the order in which the transforms are applied. Composition is not commutative. With `shift` = translate by (10, 5) and `scale` = scale by 2:

| Expression | Result | Meaning |
|---|---|---|
| `compose(scale, shift)` | `[2 0 20; 0 2 10]` | shift first, then scale, so the translation is scaled too |
| `compose(shift, scale)` | `[2 0 10; 0 2 5]` | scale first, then shift, so the translation stays as it is |

Both cases are covered in `tests/internal/test_affine.cpp`. `compose` and `to_homogeneous` are internal (`lib/src/affine.hpp`, namespace `affine_stitcher::detail`).
