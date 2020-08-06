# MobileNet v2

## Data Preparation

```bash
$ cd data/
$ python dataprepare.py --image_size=96 --valid=.2
$ python dataprepare.py --image_size=224 --valid=.2
```

## Train

```bash
# with image size 96
$ python main.py --image_size=96 --dataset=<training data> --logdir=<logdir> --modeldir=<modeldir> --train --retrain

# with image size 224
$ python main.py --image_size=224 --dataset=<training data> --logdir=<logdir> --modeldir=<modeldir> --train --retrain
```

If `OOM` occurs, half the `--batch_size`, like `--batch_size=64` / `--batch_size=32`