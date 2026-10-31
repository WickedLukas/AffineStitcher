# Test data

Images used by the tests. Tests find this folder through the `AFFINE_STITCHER_TEST_DATA_DIR` define.

## Layout

One folder per scene, holding the overlapping photos in capture order:

```
tests/data/
  <scene>/          e.g. desk-3, window-pan-4
    01.jpg
    02.jpg
    ...
    README.md       optional: camera, how it was taken, what makes the scene hard
```

## Rules

These keep the repository small, because the images live in plain git (no LFS):

- **At most 500 KB per image.** Downscale to around 1280 px on the long side and save as JPEG at quality 85–90. Use PNG only for synthetic images or images with exact pixel values.
- **A few MB in total.** Keep only scenes that test something specific (texture, little overlap, rotation, exposure changes, …).
- **Photos you took yourself**, or ones whose license allows redistribution. Note the source in the scene's README if you didn't take them.
- Strip location data (EXIF GPS) before committing.

The test `public.test data images are readable and small` checks the size limit and that OpenCV can read every `.png`/`.jpg`/`.jpeg` here.
